/*
 Copyright (c) 2013, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "can_cooperation/commands/get_interior_vehicle_data_request.h"
#include "can_cooperation/can_module_constants.h"
#include "can_cooperation/message_helper.h"
#include "can_cooperation/can_module.h"
#include "functional_module/function_ids.h"
#include "json/json.h"

namespace can_cooperation {

namespace commands {

using namespace json_keys;
using namespace message_params;

CREATE_LOGGERPTR_GLOBAL(logger_, "GetInteriorVehicleDataRequest")

GetInteriorVehicleDataRequest::GetInteriorVehicleDataRequest(
    const application_manager::MessagePtr& message,
    CANModuleInterface& can_module)
    : BaseCommandRequest(message, can_module) {}

void GetInteriorVehicleDataRequest::Execute() {
  LOG4CXX_AUTO_TRACE(logger_);

  SendRequest(functional_modules::hmi_api::get_interior_vehicle_data,
              MessageHelper::StringToValue(message_->json_message()),
              true);
}

void GetInteriorVehicleDataRequest::OnEvent(
    const can_event_engine::Event<application_manager::MessagePtr, std::string>&
        event) {
  LOG4CXX_AUTO_TRACE(logger_);
  DCHECK_OR_RETURN_VOID(
      (functional_modules::hmi_api::get_interior_vehicle_data == event.id()));
  std::string info;

  application_manager::Message& hmi_response = *(event.event_message());
  const bool validate_result = service()->ValidateMessageBySchema(hmi_response);
  Json::Value value = MessageHelper::StringToValue(hmi_response.json_message());
  std::string result_code;
  const bool success =
      validate_result && ParseResultCode(value, result_code, info);
  if (success) {
    ProccessSubscription(value);
    response_params_[kModuleData] = value[kResult][kModuleData];
  }
  SendResponse(success, result_code.c_str(), info);
}

void GetInteriorVehicleDataRequest::ProccessSubscription(
    Json::Value& hmi_response) {
  LOG4CXX_AUTO_TRACE(logger_);
  Json::Value request_params;
  Json::Reader reader;
  reader.parse(message_->json_message(), request_params);

  bool subscribe = false;
  if (IsMember(request_params, kSubscribe)) {
    subscribe = request_params[kSubscribe].asBool();
  }
  if (subscribe && !hmi_response.isMember(kIsSubscribed)) {
    LOG4CXX_WARN(logger_,
                 "conditional mandatory parameter "
                     << kIsSubscribed << " missed in hmi response");
    return;
  }

  if (!subscribe && hmi_response.isMember(kIsSubscribed)) {
    LOG4CXX_WARN(logger_,
                 "Parameter " << kIsSubscribed << " is ignored due to absence '"
                              << kSubscribe << "' parameter in request");
    return;
  }

  bool is_subscribed = response_params_[kIsSubscribed].asBool();
  response_params_[kIsSubscribed] = is_subscribed;
  CANAppExtensionPtr extension = GetAppExtension(app());
  if (is_subscribed) {
    extension->SubscribeToInteriorVehicleData(
        request_params[kModuleDescription]);
  } else {
    extension->UnsubscribeFromInteriorVehicleData(
        request_params[kModuleDescription]);
  }
}

std::string GetInteriorVehicleDataRequest::ModuleType(
    const Json::Value& message) {
  return message.get(message_params::kModuleDescription,
                     Json::Value(Json::objectValue))
      .get(message_params::kModuleType, Json::Value(""))
      .asString();
}

}  // namespace commands

}  // namespace can_cooperation
