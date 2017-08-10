/*
 * Copyright (c) 2015, Ford Motor Company
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
#ifndef TEST_COMPONENTS_FUNCTIONAL_MODULE_PLUGINS_MOCK_GENERIC_MODULE_H_
#define TEST_COMPONENTS_FUNCTIONAL_MODULE_PLUGINS_MOCK_GENERIC_MODULE_H_

#include "gmock/gmock.h"
#include "functional_module/generic_module.h"

using functional_modules::GenericModule;
using functional_modules::PluginInfo;
using functional_modules::ProcessResult;
using functional_modules::ServiceState;

using ::testing::_;
using ::testing::Return;

class MockGenericModule : public GenericModule {
 public:
  MockGenericModule();
  MOCK_CONST_METHOD0(GetPluginInfo, PluginInfo());
  MOCK_METHOD1(set_service, void(application_manager::ServicePtr service));
  MOCK_METHOD0(service, application_manager::ServicePtr());
  MOCK_METHOD1(ProcessMessage,
               ProcessResult(application_manager::MessagePtr msg));
  MOCK_METHOD1(ProcessHMIMessage,
               ProcessResult(application_manager::MessagePtr msg));
  MOCK_METHOD1(OnServiceStateChanged, void(ServiceState state));
  MOCK_METHOD1(RemoveAppExtension, void(uint32_t app_id));
  MOCK_METHOD1(IsAppForPlugin,
               bool(application_manager::ApplicationSharedPtr app));
  MOCK_METHOD2(OnAppHMILevelChanged,
               void(application_manager::ApplicationSharedPtr app,
                    mobile_apis::HMILevel::eType old_level));
  MOCK_METHOD1(OnDeviceRemoved,
               void(const connection_handler::DeviceHandle& device));
  MOCK_METHOD1(OnUnregisterApplication, void(const uint32_t app_id));
  MOCK_METHOD2(CanAppChangeHMILevel,
               bool(application_manager::ApplicationSharedPtr app,
                    mobile_apis::HMILevel::eType new_level));
  MOCK_METHOD0(RemoveAppExtensions, void());
};

#endif  // TEST_COMPONENTS_FUNCTIONAL_MODULE_PLUGINS_MOCK_GENERIC_MODULE_H_
