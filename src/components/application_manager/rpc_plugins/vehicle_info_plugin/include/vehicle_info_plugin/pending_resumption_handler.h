#ifndef PENDING_RESUMPTION_HANDLER_H
#define PENDING_RESUMPTION_HANDLER_H
#include "application_manager/event_engine/event_observer.h"
#include "vehicle_info_plugin/vehicle_info_app_extension.h"
namespace vehicle_info_plugin {
class PendingResumptionHandler
    : public application_manager::event_engine::EventObserver {
public:
  PendingResumptionHandler(
      application_manager::ApplicationManager& application_manager);

  // EventObserver interface
  void on_event(const application_manager::event_engine::Event& event);

private:
  std::map<std::string, bool> ExtractSubscribeResults(const NsSmartDeviceLink::NsSmartObjects::SmartObject& response, const NsSmartDeviceLink::NsSmartObjects::SmartObject& request) const;

  typedef std::pair <VehicleInfoAppExtension, resumption::Subscriber > FreezedResumption;
  std::vector<FreezedResumption> freezed_resumptions_;
  std::vector<std::string> pending_subscriptions_;
  std::map<uint32_t, smart_objects::SmartObject> pending_requests_;

};
}
#endif  // PENDING_RESUMPTION_HANDLER_H
