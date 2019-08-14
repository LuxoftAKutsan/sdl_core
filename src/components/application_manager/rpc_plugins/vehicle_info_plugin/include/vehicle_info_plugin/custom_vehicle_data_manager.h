#ifndef SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_VEHICLE_INFO_PLUGIN_INCLUDE_VEHICLE_INFO_PLUGIN_CUSTOM_VEHICLE_DATA_MANAGER_H_
#define SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_VEHICLE_INFO_PLUGIN_INCLUDE_VEHICLE_INFO_PLUGIN_CUSTOM_VEHICLE_DATA_MANAGER_H_
#include <set>
#include <string>

#include "application_manager/policies/custom_vehicle_data_provider.h"
#include "smart_objects/smart_object.h"

namespace vehicle_info_plugin {

class CustomVehicleDataManagerImpl;

class CustomVehicleDataManager {
 public:
  CustomVehicleDataManager(
      policy::VehicleDataItemProvider& vehicle_data_provider);
  ~CustomVehicleDataManager();

  /**
   * @brief Validates vehicle data items
   * @param msg_params received message params (msg_params key)
   * @return true, if vehicle data items within msg_params are valid,
   * otherwise - false
   */
  bool ValidateVehicleDataItems(const smart_objects::SmartObject& msg_params);

  /**
   * @brief Creates message params (nested if needed) to be sent to HMI
   * according to vehicle data item schema
   * @param item_names set of names of vehicle data items to be processed
   * @return smartMap with prepared message params
   */
  smart_objects::SmartObject CreateHMIMessageParams(
      const std::set<std::string>& item_names);

  /**
   * @brief Gets data type of vehicle data item
   * @param vehicle_data_item_name name of vehicle data
   * @return data type of vehicle_data if one found, item name otherwise
   */
  std::string GetVehicleDataItemType(
      const std::string& vehicle_data_item_name) const;

  /**
   * @brief Creates message params (nested if needed) to be sent to Mobile
   * according to vehicle data item schema
   * @param input_params message params received from hmi
   */
  void CreateMobileMessageParams(smart_objects::SmartObject& msg_params);

  /**
   * @brief Checks whether name stands for valid custom vehicle data item
   * @param name for custom vehicle data item
   */
  bool IsVehicleDataName(const std::string& name);

  /**
   * @brief Checks whether key stands for valid custom vehicle data item
   * @param key for custom vehicle data item
   */
  bool IsVehicleDataKey(const std::string& key);

 private:
  std::unique_ptr<CustomVehicleDataManagerImpl>
      custom_vehicle_data_manager_impl_;
};
}  // namespace vehicle_info_plugin
#endif  // SRC_COMPONENTS_APPLICATION_MANAGER_RPC_PLUGINS_VEHICLE_INFO_PLUGIN_INCLUDE_VEHICLE_INFO_PLUGIN_CUSTOM_VEHICLE_DATA_MANAGER_H_
