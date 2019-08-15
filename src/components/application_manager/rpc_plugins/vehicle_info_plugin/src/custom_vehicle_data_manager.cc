/**
 * Copyright (c) 2019, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "vehicle_info_plugin/custom_vehicle_data_manager.h"
#include <strings.h>
#include <limits>
#include <boost/optional.hpp>

#include "utils/helpers.h"
#include "utils/optional.h"
#include "smart_objects/smart_object.h"
#include "smart_objects/enum_schema_item.h"
#include "policy/policy_table/types.h"
#include "policy/policy_table/policy_enum_schema_factory.h"

#include "application_manager/policies/custom_vehicle_data_provider.h"
#include "application_manager/application_manager.h"
#include "application_manager/message_helper.h"
#include "interfaces/MOBILE_API_schema.h"

CREATE_LOGGERPTR_GLOBAL(logger_, "VehicleInfoPlugin")

namespace rpc {
namespace policy_table_interface_base {
class VehicleDataItem;
}
}  // namespace rpc

namespace policy {
class VehicleDataItemProvider;
}

namespace vehicle_info_plugin {

namespace policy_table = rpc::policy_table_interface_base;

typedef boost::optional<const policy_table::VehicleDataItem> OptionalDataItem;
typedef std::set<std::string> VehicleInfoSubscriptions;

enum SearchMethod { RECURSIVE = 0, NON_RECURSIVE };

template <typename Comparer>
const OptionalDataItem FindSchema(
    const std::vector<policy_table::VehicleDataItem>& oem_items,
    SearchMethod search_method,
    Comparer comparer) {
  LOG4CXX_AUTO_TRACE(logger_);

  std::vector<policy_table::VehicleDataItem> items;
  for (const auto& item : oem_items) {
    if (comparer(item)) {
      items.push_back(item);
    }
    if (search_method == SearchMethod::RECURSIVE &&
        policy_table::VehicleDataItem::kStruct == std::string(item.type)) {
      auto found_in_params =
          FindSchema(*(item.params), search_method, comparer);
      if (found_in_params.is_initialized()) {
        items.push_back(*found_in_params);
      }
    }
  }
  std::sort(items.begin(),
            items.end(),
            [](const policy_table::VehicleDataItem& left,
               const policy_table::VehicleDataItem& right) {
              if (!right.since.is_initialized()) {
                return false;
              }
              if (!left.since.is_initialized()) {
                return true;
              }
              const std::string l = *left.since;
              const std::string r = *right.since;
              return std::stof(l.c_str()) > std::stof(r.c_str());
            });

  if (!items.empty()) {
    return OptionalDataItem(*items.begin());
  }

  return OptionalDataItem();
}

template <typename T, typename U>
U GetOptional(const rpc::Optional<T>& optional, U def_val) {
  return optional.is_initialized() ? static_cast<U>(*optional) : def_val;
}


class CustomVehicleDataManagerImpl : public CustomVehicleDataManager {
 public:
  CustomVehicleDataManagerImpl(
      policy::VehicleDataItemProvider& vehicle_data_provider)
      : vehicle_data_provider_(vehicle_data_provider) {}

  bool ValidateVehicleDataItems(const smart_objects::SmartObject& msg_params) {
    LOG4CXX_AUTO_TRACE(logger_);
    const auto names = msg_params.enumerate();
    for (const auto& param_name : names) {
      auto param = msg_params[param_name];
      const auto& param_schema = FindSchemaByNameNonRecursive(param_name);
      if (!param_schema.is_initialized()) {
        LOG4CXX_ERROR(logger_,
                      "Unable to find schema for param : " << param_name);
        return false;
      }
      if (!ValidateVehicleDataItem(param, *param_schema)) {
        LOG4CXX_ERROR(logger_, "Item validation failed: " << param_name);
        return false;
      }
    }
    return true;
  }

  void CreateMobileMessageParams(smart_objects::SmartObject& msg_params) {
    using namespace application_manager;
    LOG4CXX_AUTO_TRACE(logger_);

    typedef std::function<smart_objects::SmartObject(
        const smart_objects::SmartObject& input_params,
        SearchMethod search_method)>
        MobileMsgConstructor;

    MobileMsgConstructor fill_mobile_msg =
        [this, &fill_mobile_msg](
            const smart_objects::SmartObject& input_params,
            SearchMethod search_method) -> smart_objects::SmartObject {
      smart_objects::SmartObject out_params;

      const auto& items = input_params.enumerate();
      for (const auto& key : items) {
        auto schema = search_method == SearchMethod::RECURSIVE
                          ? FindSchemaByKeyRecursive(key)
                          : FindSchemaByKeyNonRecursive(key);
        if (!schema.is_initialized()) {
          LOG4CXX_DEBUG(logger_, "Schema for: " << key << " cannot be found");
          continue;
        }

        const auto& item_name = schema->name;
        if (policy_table::VehicleDataItem::kStruct ==
            std::string(schema->type)) {
          const auto param =
              fill_mobile_msg(input_params[key], SearchMethod::RECURSIVE);
          if (!param.empty()) {
            out_params[item_name] = param;
            continue;
          }
        }

        out_params[item_name] = input_params[key];
      }

      return out_params;
    };
    const auto& rpc_spec_vehicle_data = MessageHelper::vehicle_data();
    const auto& item_names = msg_params.enumerate();
    smart_objects::SmartObject params_to_convert;

    for (const auto& item_key : item_names) {
      const auto& found_it = rpc_spec_vehicle_data.find(item_key);
      if (found_it == rpc_spec_vehicle_data.end()) {
        params_to_convert[item_key] = msg_params[item_key];
        msg_params.erase(item_key);
      }
    }

    const auto& mobile_msg_params =
        fill_mobile_msg(params_to_convert, SearchMethod::NON_RECURSIVE);
    for (const auto& name : mobile_msg_params.enumerate()) {
      msg_params[name] = mobile_msg_params[name];
    }
  }

  smart_objects::SmartObject CreateHMIMessageParams(
      const std::set<std::string>& item_names) {
    typedef std::function<smart_objects::SmartObject(
        const policy_table::VehicleDataItem&)>
        ParamsConstructor;

    auto fill_param = [](ParamsConstructor& constructor,
                         const policy_table::VehicleDataItem& param,
                         smart_objects::SmartObject* out_params) {
      DCHECK_OR_RETURN_VOID(out_params)
      const auto param_key = std::string(param.key);
      const auto param_type = std::string(param.type);
      if (policy_table::VehicleDataItem::kStruct == param_type) {
        (*out_params)[param_key] = constructor(param);
      } else {
        (*out_params)[param_key] = true;
      }
    };

    ParamsConstructor fill_hmi_params =
        [&fill_hmi_params,
         fill_param](const policy_table::VehicleDataItem& item) {
          smart_objects::SmartObject out_params;
          DCHECK(policy_table::VehicleDataItem::kStruct ==
                 std::string(item.type));
          for (auto& param : *(item.params)) {
            fill_param(fill_hmi_params, param, &out_params);
          }
          return out_params;
        };

    smart_objects::SmartObject out_params(smart_objects::SmartType_Map);
    for (const auto& name : item_names) {
      auto schema = FindSchemaByNameNonRecursive(name);
      if (schema.is_initialized()) {
        fill_param(fill_hmi_params, *schema, &out_params);
      }
    }

    return out_params;
  }

  std::string GetVehicleDataItemType(
      const std::string& vehicle_data_item_name) const  {
    LOG4CXX_AUTO_TRACE(logger_);
    const auto& schema = FindSchemaByNameNonRecursive(vehicle_data_item_name);

    return schema.is_initialized() ? std::string(schema->type)
                                   : vehicle_data_item_name;
  }

  bool IsVehicleDataName(const std::string& name) {
    LOG4CXX_AUTO_TRACE(logger_);
    using namespace application_manager;

    const auto& rpc_spec_vehicle_data = MessageHelper::vehicle_data();
    const auto& item_is_rpc_spec = rpc_spec_vehicle_data.find(name);
    if (item_is_rpc_spec != rpc_spec_vehicle_data.end()) {
      return true;
    }

    const auto& schema = FindSchemaByNameNonRecursive(name);
    if (!schema) {
      return false;
    }
    return schema->is_initialized();
  }

  bool IsVehicleDataKey(const std::string& key) {
    LOG4CXX_AUTO_TRACE(logger_);
    const auto& schema = FindSchemaByKeyNonRecursive(key);
    if (!schema) {
      return false;
    }
    return schema->is_initialized();
  }

 private:
  bool ValidateVehicleDataItem(
      const smart_objects::SmartObject& item,
      const policy_table::VehicleDataItem& item_schema) {
    LOG4CXX_AUTO_TRACE(logger_);
    auto validate_array = [this, &item, &item_schema]() -> bool {
      const auto minsize = GetOptional(item_schema.minsize, size_t(0));
      const auto maxsize =
          GetOptional(item_schema.maxsize, std::numeric_limits<size_t>::max());

      const auto items_array = item.asArray();
      if (!items_array) {
        auto allowed_types = {smart_objects::SmartType::SmartType_Array,
                              smart_objects::SmartType::SmartType_Map};
        if (!helpers::in_range(allowed_types, item.getType())) {
          LOG4CXX_WARN(
              logger_,
              "Type mismatch. Expected: array, actual: " << item.getType());
          return false;
        }
      }

      const auto array_size = NULL == items_array ? 0 : (*items_array).size();
      if (array_size < minsize || array_size > maxsize) {
        LOG4CXX_WARN(logger_,
                     "array size : " << array_size
                                     << " does not fit into boundaries : ("
                                     << minsize << " , " << maxsize << ")");
        return false;
      }

      for (uint64_t i = 0; i < array_size; ++i) {
        policy_table::VehicleDataItem not_array_schema(item_schema);
        not_array_schema.array = rpc::Optional<rpc::Boolean>(false);
        if (!ValidateVehicleDataItem((*items_array)[i], not_array_schema)) {
          LOG4CXX_WARN(
              logger_,
              "Param validation failed : " << std::string(item_schema.name));
          return false;
        }
      }
      return true;
    };

    if (item_schema.mandatory &&
        smart_objects::SmartType::SmartType_Null == item.getType()) {
      LOG4CXX_WARN(
          logger_,
          "Mandatory item does not exist : " << std::string(item_schema.name));
      return false;
    }

    if (item_schema.array.is_initialized() && *item_schema.array) {
      return validate_array();
    }

    const std::string type = std::string(item_schema.type);
    if (policy_table::VehicleDataItem::kStruct == type) {
      return ValidateStructTypeItem(item, item_schema);
    }
    if (helpers::in_range(policy_table::VehicleDataItem::kPODTypes, type)) {
      return ValidatePODTypeItem(item, item_schema);
    }
    return ValidateRPCSpecEnumVehicleDataItem(item, item_schema);
  }

  /**
   * @brief ValidateRPCSpecEnumVehicleDataItem validate custom item in case it
   * `type` of the item is enum from RPCspec
   * @param item item to validate
   * @param item_schema schema for item validation
   * @return true if item is valid according schema, false if not
   */
  bool ValidateRPCSpecEnumVehicleDataItem(
      const smart_objects::SmartObject& item,
      const policy_table::VehicleDataItem& item_schema) {
    LOG4CXX_AUTO_TRACE(logger_);
    using namespace ns_smart_device_link::ns_smart_objects;
    using rpc::policy_table_interface_base::EnumSchemaItemFactory;

    auto type = std::string(item_schema.type);
    const auto schema_item = EnumSchemaItemFactory::Get(type);
    if (!schema_item) {
      LOG4CXX_WARN(logger_, "Unable to find schema item for" << type);
      return false;
    }
    rpc::ValidationReport report(type);
    smart_objects::SmartObject item_copy = item;
    schema_item->applySchema(item_copy, false);
    if (-1 == item_copy.asInt()) {
      LOG4CXX_WARN(logger_,
                   "Enum <"
                       << std::string(item_schema.name)
                       << "> schema application failed : " << item.asString());
      return false;
    }

    errors::eType validation_result = schema_item->validate(item_copy, &report);
    if (errors::OK != validation_result) {
      LOG4CXX_WARN(logger_,
                   "Enum <" << std::string(item_schema.name)
                            << "> validation failed : " << item.asString());
      return false;
    }
    return true;
  }

  /**
   * @brief ValidatePODTypeItem validate custom item in case it
   * `type` of the item Plain Old Data (POD)
   * @param item item to validate
   * @param item_schema schema for item validation
   * @return true if item is valid according schema, false if not
   */
  bool ValidatePODTypeItem(const smart_objects::SmartObject& item,
                           const policy_table::VehicleDataItem& item_schema) {
    LOG4CXX_AUTO_TRACE(logger_);
    const std::string item_type = std::string(item_schema.type);
    DCHECK_OR_RETURN(
        helpers::in_range(policy_table::VehicleDataItem::kPODTypes, item_type),
        false);
    bool result = true;
    if (item_type == policy_table::VehicleDataItem::kString) {
      if (smart_objects::SmartType::SmartType_String != item.getType()) {
        LOG4CXX_WARN(logger_,
                     "Parameter type mismatch. Expected: "
                         << policy_table::VehicleDataItem::kString
                         << ", actual: " << item.getType());
        return false;
      }

      auto value = item.asString();
      const auto length = value.size();
      const auto minlength = GetOptional(item_schema.minlength, size_t(0));
      const auto maxlength = GetOptional(item_schema.maxlength,
                                         std::numeric_limits<size_t>::max());
      result = (minlength <= length) && (maxlength >= length);
      if (!result) {
        LOG4CXX_WARN(
            logger_,
            "String size : " << length << " does not fit into boundaries : ("
                             << minlength << " , " << maxlength << ")");
      }
    } else if (item_type == policy_table::VehicleDataItem::kInteger) {
      auto numeric_types = {smart_objects::SmartType::SmartType_Integer,
                            smart_objects::SmartType::SmartType_UInteger};
      if (!helpers::in_range(numeric_types, item.getType())) {
        LOG4CXX_WARN(logger_,
                     "Parameter type mismatch. Expected: "
                         << policy_table::VehicleDataItem::kInteger
                         << ", actual: " << item.getType());
        return false;
      }
      auto value = item.asInt();
      const auto minval = GetOptional(item_schema.minvalue,
                                      std::numeric_limits<int64_t>::min());
      const auto maxval = GetOptional(item_schema.maxvalue,
                                      std::numeric_limits<int64_t>::max());
      result = (minval <= value) && (maxval >= value);
      if (!result) {
        LOG4CXX_WARN(logger_,
                     "Integer value: " << value
                                       << " does not fit into boundaries : ("
                                       << minval << " , " << maxval << ")");
      }
    } else if (item_type == policy_table::VehicleDataItem::kFloat) {
      auto numeric_types = {smart_objects::SmartType::SmartType_Double,
                            smart_objects::SmartType::SmartType_Integer,
                            smart_objects::SmartType::SmartType_UInteger};
      if (!helpers::in_range(numeric_types, item.getType())) {
        LOG4CXX_WARN(logger_,
                     "Parameter type mismatch. Expected: "
                         << policy_table::VehicleDataItem::kFloat
                         << ", actual: " << item.getType());
        return false;
      }
      auto value = item.asDouble();
      const auto minval =
          GetOptional(item_schema.minvalue, std::numeric_limits<double>::min());
      const auto maxval =
          GetOptional(item_schema.maxvalue, std::numeric_limits<double>::max());
      result = (minval <= value) && (maxval >= value);
      if (!result) {
        LOG4CXX_WARN(logger_,
                     "Float value: " << value
                                     << " does not fit into boundaries : ("
                                     << minval << " , " << maxval << ")");
      }
    } else if (item_type == policy_table::VehicleDataItem::kBoolean) {
      if (smart_objects::SmartType::SmartType_Boolean != item.getType()) {
        LOG4CXX_WARN(logger_,
                     "Parameter type mismatch. Expected: "
                         << policy_table::VehicleDataItem::kBoolean
                         << ", actual: " << item.getType());
        return false;
      }
    }
    return result;
  }

  bool ValidateStructTypeItem(
      const ns_smart_device_link::ns_smart_objects::SmartObject& item,
      const policy_table::VehicleDataItem& item_schema) {
    LOG4CXX_AUTO_TRACE(logger_);

    // Check for redundant parameters
    for (const auto& param : item.enumerate()) {
      auto param_schema = FindSchemaByNameRecursive(param);
      if (!param_schema.is_initialized()) {
        return false;
      }
    }

    auto params_schemas = *(item_schema.params);
    for (const auto& param_schema : params_schemas) {
      if (!item.keyExists(param_schema.name)) {
        if (param_schema.mandatory) {
          return false;
        }
        continue;
      }

      if (!ValidateVehicleDataItem(item[param_schema.name], param_schema)) {
        return false;
      }
    }
    return true;
  }

  const OptionalDataItem FindSchemaByNameNonRecursive(
      const std::string& name) const {
    LOG4CXX_AUTO_TRACE(logger_);

    auto& oem_items = vehicle_data_provider_.GetVehicleDataItems();
    auto compare_by_name = [&name](const policy_table::VehicleDataItem& item) {
      return (name == std::string(item.name));
    };

    return FindSchema(oem_items, SearchMethod::NON_RECURSIVE, compare_by_name);
  }


    const OptionalDataItem FindSchemaByKeyNonRecursive(const std::string& key)
        const {
      LOG4CXX_AUTO_TRACE(logger_);

      auto& oem_items = vehicle_data_provider_.GetVehicleDataItems();
      auto compare_by_key = [&key](const policy_table::VehicleDataItem& item) {
        return (key == std::string(item.key));
      };

      return FindSchema(oem_items, SearchMethod::NON_RECURSIVE, compare_by_key);
    }
    const OptionalDataItem FindSchemaByNameRecursive(const std::string& name)
        const {
      LOG4CXX_AUTO_TRACE(logger_);

      auto& oem_items = vehicle_data_provider_.GetVehicleDataItems();
      auto compare_by_name =
          [&name](const policy_table::VehicleDataItem& item) {
            return (name == std::string(item.name));
          };

      return FindSchema(oem_items, SearchMethod::RECURSIVE, compare_by_name);
    }

    const OptionalDataItem FindSchemaByKeyRecursive(const std::string& key)
        const

    {
      LOG4CXX_AUTO_TRACE(logger_);

      auto& oem_items = vehicle_data_provider_.GetVehicleDataItems();
      auto compare_by_key = [&key](const policy_table::VehicleDataItem& item) {
        return (key == std::string(item.key));
      };

      return FindSchema(oem_items, SearchMethod::RECURSIVE, compare_by_key);
    }

    policy::VehicleDataItemProvider& vehicle_data_provider_;
  };

  CustomVehicleDataManager::CustomVehicleDataManager() {
  }

  CustomVehicleDataManager::~CustomVehicleDataManager() = default;

  CustomVehicleDataManager* CustomVehicleDataManager::create(policy::VehicleDataItemProvider& vehicle_data_provider) {
    return new CustomVehicleDataManagerImpl(vehicle_data_provider);
  }

  CustomVehicleDataManager* CustomVehicleDataManager::create(CustomVehicleDataManager* impl) {
    auto instance = new CustomVehicleDataManager();
    instance->custom_vehicle_data_manager_impl_.reset(impl);
    return instance;
  }

  bool CustomVehicleDataManager::ValidateVehicleDataItems(
      const smart_objects::SmartObject& msg_params) {
    return custom_vehicle_data_manager_impl_->ValidateVehicleDataItems(
        msg_params);
  };

  smart_objects::SmartObject CustomVehicleDataManager::CreateHMIMessageParams(
      const std::set<std::string>& item_names) {
    return custom_vehicle_data_manager_impl_->CreateHMIMessageParams(
        item_names);
  };

  std::string CustomVehicleDataManager::GetVehicleDataItemType(
      const std::string& vehicle_data_item_name) const {
    return custom_vehicle_data_manager_impl_->GetVehicleDataItemType(
        vehicle_data_item_name);
  };

  void CustomVehicleDataManager::CreateMobileMessageParams(
      smart_objects::SmartObject& msg_params) {
    return custom_vehicle_data_manager_impl_->CreateMobileMessageParams(
        msg_params);
  };

  bool CustomVehicleDataManager::IsVehicleDataName(const std::string& name) {
    return custom_vehicle_data_manager_impl_->IsVehicleDataName(name);
  };

  bool CustomVehicleDataManager::IsVehicleDataKey(const std::string& key) {
    return custom_vehicle_data_manager_impl_->IsVehicleDataKey(key);
  };

};  // namespace vehicle_info_plugin
