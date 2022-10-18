/**
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nlohmann/json.hpp"

#include "srf/segment/object.hpp"

#include <map>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#pragma once

namespace srf::segment {
class Builder;
}

namespace srf::modules {

class SegmentModule
{
  public:
    friend segment::Builder;

    using segment_module_port_map_t     = std::map<std::string, std::shared_ptr<segment::ObjectProperties>>;
    using segment_module_port_t         = std::shared_ptr<segment::ObjectProperties>;
    using segment_module_typeinfo_map_t = std::map<std::string, const std::type_info*>;

    SegmentModule() = delete;
    SegmentModule(std::string module_name);
    SegmentModule(std::string module_name, nlohmann::json config);

    const std::string& component_prefix() const;
    const nlohmann::json& config() const;
    const std::string& name() const;

    std::string get_module_component_name(const std::string& component_name) const;

    /**
     * Return vector of input names -- these are only understood by the SegmentModule
     * @return std::vector
     */
    const std::vector<std::string> input_ids() const;

    /**
     * Return a vector of output names -- these are only understood by the SegmentModule class
     * @return std::vector
     */
    const std::vector<std::string> output_ids() const;

    /**
     * Return a set of ObjectProperties for module input_ids
     * @return ObjectProperties
     */
    const segment_module_port_map_t input_ports() const;

    /**
     * Return the ObjectProperties object corresponding to input_name
     * @param input_name Name of the module port
     * @return ObjectProperties
     */
    segment_module_port_t input_port(const std::string& input_name);

    /**
     * Return a map of module port id : type indices
     * @return std::map
     */
    const std::map<std::string, const std::type_info*> input_port_type_ids() const;

    /**
     * Return the type index of a given input name
     * @return Type index
     */
    const std::type_info* input_port_type_id(const std::string& input_name);

    /**
     * Return a set of ObjectProperties for module input_ids
     * @return ObjectProperties
     */
    const segment_module_port_map_t output_ports() const;

    /**
     * Return an ObjectProperties for the module port corresponding to output name
     * @param output_name Name of the module port to return
     * @return ObjectProperties
     */
    segment_module_port_t output_port(const std::string& output_name);

    /**
     * Return a map of module port id : type indices
     * @return std::map
     */
    const segment_module_typeinfo_map_t output_port_type_ids() const;

    /**
     * Return the type index of a given input name
     * @param input_name Name of the module port to return a type index for
     * @return Type index
     */
    const std::type_info* output_port_type_id(const std::string& output_name);

    /**
     * Functional entrypoint for module constructor during build -- this lets us act like a std::function
     * @param builder
     */
    void operator()(segment::Builder& builder)
    {
        this->initialize(builder);
    };

    /* Virtual Functions */
    /**
     * Entrypoint for module constructor during build
     * @param builder
     */
    virtual void initialize(segment::Builder& builder) = 0;

  protected:
    // Derived class interface functions

    /**
     * Register an input port that should be exposed for the module
     * @param input_name Port name
     * @param object ObjectProperties object associated with the port
     * @param tinfo type_info pointer for the data type expected by the input
     */
    void register_input_port(std::string input_name,
                             std::shared_ptr<segment::ObjectProperties> object,
                             const std::type_info* tinfo);

    /**
     * Register an output port that should be exposed for the module
     * @param input_name Port name
     * @param object ObjectProperties object assocaited with the port
     * @param tinfo type_info pointer for the date type emitted by the output
     */
    void register_output_port(std::string input_name,
                              std::shared_ptr<segment::ObjectProperties> object,
                              const std::type_info* tinfo);

  private:
    std::string m_module_name;
    std::string m_component_prefix;

    std::vector<std::string> m_input_port_ids{};
    std::vector<std::string> m_output_port_ids{};

    segment_module_typeinfo_map_t m_input_port_type_indices{};
    segment_module_typeinfo_map_t m_output_port_type_indices{};

    segment_module_port_map_t m_input_ports{};
    segment_module_port_map_t m_output_ports{};

    const nlohmann::json m_config;
};

}  // namespace srf::modules