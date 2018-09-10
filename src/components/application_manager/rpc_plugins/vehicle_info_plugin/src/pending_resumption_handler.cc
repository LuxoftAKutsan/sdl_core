#include "vehicle_info_plugin/pending_resumption_handler.h"

namespace vehicle_info_plugin {

PendingResumptionHandler::PendingResumptionHandler(
    application_manager::ApplicationManager& application_manager) {}

template <class T, class Container>
bool Contains(const T& val, const Container& container) {
  container.find(val) != container.end();
}

void PendingResumptionHandler::on_event(
    const application_manager::event_engine::Event& event) {
  const smart_objects::SmartObject& response = event.smart_object();
  const uint32_t corr_id = correlation_id = event.smart_object_correlation_id();
  const auto request = pending_requests_[corr_id];
  std::map<std::string, bool> subscription_results = ExtractSubscribeResults(request, response);

  for (auto ivi_status : subscription_results) {
     // Find Application extensions that were subscribed to ivi and finish their resumption
  }
}

std::map<std::string, bool> PendingResumptionHandler::ExtractSubscribeResults(
        const smart_objects::SmartObject& response,
        const smart_objects::SmartObject& request) const {
    const hmi_apis::Common_Result::eType result_code =
        static_cast<hmi_apis::Common_Result::eType>(
            response[strings::params][application_manager::hmi_response::code]
                .asInt());
    bool succesfull_response = (result_code == hmi_apis::Common_Result::SUCCESS ||
                                result_code == hmi_apis::Common_Result::WARNINGS);
    const auto response_keys = response[strings::msg_params].enumerate();
    const auto request_keys = request[strings::msg_params].enumerate();

    std::map<std::string, bool> subscription_results;

    if (!succesfull_response) {
      for (auto key : request_keys) {
        subscription_results[key] = false;
      }
    }

    if (succesfull_response) {
      for (auto key : request_keys) {
          if (!Contains(key, response_keys)) {
              subscription_results[key] = true;
          } else {
              const auto kSuccess = hmi_apis::Common_VehicleDataResultCode::VDRC_SUCCESS;
              const auto vd_result_code =  response_params[ivi][strings::result_code].asInt();
              subscription_results[key] = vd_result_code == kSuccess;
          }
      }
    }
    return subscription_results;
}
}
