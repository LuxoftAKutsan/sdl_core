// This file is generated, do not edit
#include "policy/policy_table/enums.h"

namespace rpc {
namespace policy_table_interface_base {

bool IsValidEnum(Parameter val) {
  switch (val) {
    case P_GPS:
      return true;
    case P_SPEED:
      return true;
    case P_ENGINETORQUE:
      return true;
    case P_EXTERNALTEMPERATURE:
      return true;
    case P_FUELLEVEL:
      return true;
    case P_FUELLEVEL_STATE:
      return true;
    case P_HEADLAMPSTATUS:
      return true;
    case P_INSTANTFUELCONSUMPTION:
      return true;
    case P_FUELRANGE:
      return true;
    case P_ODOMETER:
      return true;
    case P_TIREPRESSURE:
      return true;
    case P_WIPERSTATUS:
      return true;
    case P_VIN:
      return true;
    case P_ACCPEDALPOSITION:
      return true;
    case P_BELTSTATUS:
      return true;
    case P_DRIVERBRAKING:
      return true;
    case P_PRNDL:
      return true;
    case P_RPM:
      return true;
    case P_STEERINGWHEELANGLE:
      return true;
    case P_ENGINEOILLIFE:
      return true;
    case P_MYKEY:
      return true;
    case P_AIRBAGSTATUS:
      return true;
    case P_BODYINFORMATION:
      return true;
    case P_CLUSTERMODESTATUS:
      return true;
    case P_DEVICESTATUS:
      return true;
    case P_EMERGENCYEVENT:
      return true;
    case P_ECALLINFO:
      return true;
    case P_EMPTY:
      return true;
    default:
      return false;
  }
}
const char* EnumToJsonString(Parameter val) {
  switch (val) {
    case P_GPS:
      return "gps";
    case P_SPEED:
      return "speed";
    case P_ENGINETORQUE:
      return "engineTorque";
    case P_EXTERNALTEMPERATURE:
      return "externalTemperature";
    case P_FUELLEVEL:
      return "fuelLevel";
    case P_FUELLEVEL_STATE:
      return "fuelLevel_State";
    case P_HEADLAMPSTATUS:
      return "headLampStatus";
    case P_INSTANTFUELCONSUMPTION:
      return "instantFuelConsumption";
    case P_FUELRANGE:
      return "fuelRange";
    case P_ODOMETER:
      return "odometer";
    case P_TIREPRESSURE:
      return "tirePressure";
    case P_WIPERSTATUS:
      return "wiperStatus";
    case P_VIN:
      return "vin";
    case P_ACCPEDALPOSITION:
      return "accPedalPosition";
    case P_BELTSTATUS:
      return "beltStatus";
    case P_DRIVERBRAKING:
      return "driverBraking";
    case P_PRNDL:
      return "prndl";
    case P_RPM:
      return "rpm";
    case P_STEERINGWHEELANGLE:
      return "steeringWheelAngle";
    case P_ENGINEOILLIFE:
      return "engineOilLife";
    case P_MYKEY:
      return "myKey";
    case P_AIRBAGSTATUS:
      return "airbagStatus";
    case P_BODYINFORMATION:
      return "bodyInformation";
    case P_CLUSTERMODESTATUS:
      return "clusterModeStatus";
    case P_DEVICESTATUS:
      return "deviceStatus";
    case P_EMERGENCYEVENT:
      return "emergencyEvent";
    case P_ECALLINFO:
      return "eCallInfo";
    case P_EMPTY:
      return "EMPTY";
    default:
      return "";
  }
}

bool EnumFromJsonString(const std::string& literal, Parameter* result) {
  if ("gps" == literal) {
    *result = P_GPS;
    return true;
  } else if ("speed" == literal) {
    *result = P_SPEED;
    return true;
  } else if ("engineTorque" == literal) {
    *result = P_ENGINETORQUE;
    return true;
  } else if ("externalTemperature" == literal) {
    *result = P_EXTERNALTEMPERATURE;
    return true;
  } else if ("fuelLevel" == literal) {
    *result = P_FUELLEVEL;
    return true;
  } else if ("fuelLevel_State" == literal) {
    *result = P_FUELLEVEL_STATE;
    return true;
  } else if ("headLampStatus" == literal) {
    *result = P_HEADLAMPSTATUS;
    return true;
  } else if ("instantFuelConsumption" == literal) {
    *result = P_INSTANTFUELCONSUMPTION;
    return true;
  } else if ("fuelRange" == literal) {
    *result = P_FUELRANGE;
    return true;
  } else if ("odometer" == literal) {
    *result = P_ODOMETER;
    return true;
  } else if ("tirePressure" == literal) {
    *result = P_TIREPRESSURE;
    return true;
  } else if ("wiperStatus" == literal) {
    *result = P_WIPERSTATUS;
    return true;
  } else if ("vin" == literal) {
    *result = P_VIN;
    return true;
  } else if ("accPedalPosition" == literal) {
    *result = P_ACCPEDALPOSITION;
    return true;
  } else if ("beltStatus" == literal) {
    *result = P_BELTSTATUS;
    return true;
  } else if ("driverBraking" == literal) {
    *result = P_DRIVERBRAKING;
    return true;
  } else if ("prndl" == literal) {
    *result = P_PRNDL;
    return true;
  } else if ("rpm" == literal) {
    *result = P_RPM;
    return true;
  } else if ("steeringWheelAngle" == literal) {
    *result = P_STEERINGWHEELANGLE;
    return true;
  } else if ("engineOilLife" == literal) {
    *result = P_ENGINEOILLIFE;
    return true;
  } else if ("myKey" == literal) {
    *result = P_MYKEY;
    return true;
  } else if ("airbagStatus" == literal) {
    *result = P_AIRBAGSTATUS;
    return true;
  } else if ("bodyInformation" == literal) {
    *result = P_BODYINFORMATION;
    return true;
  } else if ("clusterModeStatus" == literal) {
    *result = P_CLUSTERMODESTATUS;
    return true;
  } else if ("deviceStatus" == literal) {
    *result = P_DEVICESTATUS;
    return true;
  } else if ("emergencyEvent" == literal) {
    *result = P_EMERGENCYEVENT;
    return true;
  } else if ("eCallInfo" == literal) {
    *result = P_ECALLINFO;
    return true;
  } else if ("EMPTY" == literal) {
    *result = P_EMPTY;
    return true;
  } else {
    return false;
  }
}

const std::string kDefaultApp = "default";
const std::string kPreDataConsentApp = "pre_DataConsent";
const std::string kDeviceApp = "device";

}  // namespace policy_table_interface_base
}  // namespace rpc
