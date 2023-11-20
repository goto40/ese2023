// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file Simple.cpp
 * This source file contains the definition of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "Simple.h"
#include <fastcdr/Cdr.h>

#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>

#define dds_examples_ComplexT_max_cdr_typesize 96ULL;

#define dds_examples_SimpleT_max_cdr_typesize 24ULL;
#define dds_examples_ComplexT_max_key_cdr_typesize 0ULL;

#define dds_examples_SimpleT_max_key_cdr_typesize 0ULL;

dds_examples::SimpleT::SimpleT()
{
    // long long m_j
    m_j = 0;
    // long long m_i
    m_i = 0;
    // double m_f
    m_f = 0.0;

}

dds_examples::SimpleT::~SimpleT()
{



}

dds_examples::SimpleT::SimpleT(
        const SimpleT& x)
{
    m_j = x.m_j;
    m_i = x.m_i;
    m_f = x.m_f;
}

dds_examples::SimpleT::SimpleT(
        SimpleT&& x) noexcept 
{
    m_j = x.m_j;
    m_i = x.m_i;
    m_f = x.m_f;
}

dds_examples::SimpleT& dds_examples::SimpleT::operator =(
        const SimpleT& x)
{

    m_j = x.m_j;
    m_i = x.m_i;
    m_f = x.m_f;

    return *this;
}

dds_examples::SimpleT& dds_examples::SimpleT::operator =(
        SimpleT&& x) noexcept
{

    m_j = x.m_j;
    m_i = x.m_i;
    m_f = x.m_f;

    return *this;
}

bool dds_examples::SimpleT::operator ==(
        const SimpleT& x) const
{

    return (m_j == x.m_j && m_i == x.m_i && m_f == x.m_f);
}

bool dds_examples::SimpleT::operator !=(
        const SimpleT& x) const
{
    return !(*this == x);
}

size_t dds_examples::SimpleT::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return dds_examples_SimpleT_max_cdr_typesize;
}

size_t dds_examples::SimpleT::getCdrSerializedSize(
        const dds_examples::SimpleT& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += 8 + eprosima::fastcdr::Cdr::alignment(current_alignment, 8);


    current_alignment += 8 + eprosima::fastcdr::Cdr::alignment(current_alignment, 8);


    current_alignment += 8 + eprosima::fastcdr::Cdr::alignment(current_alignment, 8);



    return current_alignment - initial_alignment;
}

void dds_examples::SimpleT::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_j;scdr << m_i;scdr << m_f;
}

void dds_examples::SimpleT::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_j;
    dcdr >> m_i;
    dcdr >> m_f;
}

/*!
 * @brief This function sets a value in member j
 * @param _j New value for member j
 */
void dds_examples::SimpleT::j(
        int64_t _j)
{
    m_j = _j;
}

/*!
 * @brief This function returns the value of member j
 * @return Value of member j
 */
int64_t dds_examples::SimpleT::j() const
{
    return m_j;
}

/*!
 * @brief This function returns a reference to member j
 * @return Reference to member j
 */
int64_t& dds_examples::SimpleT::j()
{
    return m_j;
}

/*!
 * @brief This function sets a value in member i
 * @param _i New value for member i
 */
void dds_examples::SimpleT::i(
        int64_t _i)
{
    m_i = _i;
}

/*!
 * @brief This function returns the value of member i
 * @return Value of member i
 */
int64_t dds_examples::SimpleT::i() const
{
    return m_i;
}

/*!
 * @brief This function returns a reference to member i
 * @return Reference to member i
 */
int64_t& dds_examples::SimpleT::i()
{
    return m_i;
}

/*!
 * @brief This function sets a value in member f
 * @param _f New value for member f
 */
void dds_examples::SimpleT::f(
        double _f)
{
    m_f = _f;
}

/*!
 * @brief This function returns the value of member f
 * @return Value of member f
 */
double dds_examples::SimpleT::f() const
{
    return m_f;
}

/*!
 * @brief This function returns a reference to member f
 * @return Reference to member f
 */
double& dds_examples::SimpleT::f()
{
    return m_f;
}



size_t dds_examples::SimpleT::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return dds_examples_SimpleT_max_key_cdr_typesize;
}

bool dds_examples::SimpleT::isKeyDefined()
{
    return false;
}

void dds_examples::SimpleT::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
}



dds_examples::ComplexT::ComplexT()
{
    // dds_examples::SimpleT m_s

    // dds_examples::SimpleArray3T m_a


}

dds_examples::ComplexT::~ComplexT()
{


}

dds_examples::ComplexT::ComplexT(
        const ComplexT& x)
{
    m_s = x.m_s;
    m_a = x.m_a;
}

dds_examples::ComplexT::ComplexT(
        ComplexT&& x) noexcept 
{
    m_s = std::move(x.m_s);
    m_a = std::move(x.m_a);
}

dds_examples::ComplexT& dds_examples::ComplexT::operator =(
        const ComplexT& x)
{

    m_s = x.m_s;
    m_a = x.m_a;

    return *this;
}

dds_examples::ComplexT& dds_examples::ComplexT::operator =(
        ComplexT&& x) noexcept
{

    m_s = std::move(x.m_s);
    m_a = std::move(x.m_a);

    return *this;
}

bool dds_examples::ComplexT::operator ==(
        const ComplexT& x) const
{

    return (m_s == x.m_s && m_a == x.m_a);
}

bool dds_examples::ComplexT::operator !=(
        const ComplexT& x) const
{
    return !(*this == x);
}

size_t dds_examples::ComplexT::getMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return dds_examples_ComplexT_max_cdr_typesize;
}

size_t dds_examples::ComplexT::getCdrSerializedSize(
        const dds_examples::ComplexT& data,
        size_t current_alignment)
{
    (void)data;
    size_t initial_alignment = current_alignment;


    current_alignment += dds_examples::SimpleT::getCdrSerializedSize(data.s(), current_alignment);

    for(size_t a = 0; a < data.a().size(); ++a)
    {

            current_alignment += dds_examples::SimpleT::getCdrSerializedSize(data.a().at(a), current_alignment);
    }

    return current_alignment - initial_alignment;
}

void dds_examples::ComplexT::serialize(
        eprosima::fastcdr::Cdr& scdr) const
{

    scdr << m_s;scdr << m_a;

}

void dds_examples::ComplexT::deserialize(
        eprosima::fastcdr::Cdr& dcdr)
{

    dcdr >> m_s;
    dcdr >> m_a;

}

/*!
 * @brief This function copies the value in member s
 * @param _s New value to be copied in member s
 */
void dds_examples::ComplexT::s(
        const dds_examples::SimpleT& _s)
{
    m_s = _s;
}

/*!
 * @brief This function moves the value in member s
 * @param _s New value to be moved in member s
 */
void dds_examples::ComplexT::s(
        dds_examples::SimpleT&& _s)
{
    m_s = std::move(_s);
}

/*!
 * @brief This function returns a constant reference to member s
 * @return Constant reference to member s
 */
const dds_examples::SimpleT& dds_examples::ComplexT::s() const
{
    return m_s;
}

/*!
 * @brief This function returns a reference to member s
 * @return Reference to member s
 */
dds_examples::SimpleT& dds_examples::ComplexT::s()
{
    return m_s;
}
/*!
 * @brief This function copies the value in member a
 * @param _a New value to be copied in member a
 */
void dds_examples::ComplexT::a(
        const dds_examples::SimpleArray3T& _a)
{
    m_a = _a;
}

/*!
 * @brief This function moves the value in member a
 * @param _a New value to be moved in member a
 */
void dds_examples::ComplexT::a(
        dds_examples::SimpleArray3T&& _a)
{
    m_a = std::move(_a);
}

/*!
 * @brief This function returns a constant reference to member a
 * @return Constant reference to member a
 */
const dds_examples::SimpleArray3T& dds_examples::ComplexT::a() const
{
    return m_a;
}

/*!
 * @brief This function returns a reference to member a
 * @return Reference to member a
 */
dds_examples::SimpleArray3T& dds_examples::ComplexT::a()
{
    return m_a;
}


size_t dds_examples::ComplexT::getKeyMaxCdrSerializedSize(
        size_t current_alignment)
{
    static_cast<void>(current_alignment);
    return dds_examples_ComplexT_max_key_cdr_typesize;
}

bool dds_examples::ComplexT::isKeyDefined()
{
    return false;
}

void dds_examples::ComplexT::serializeKey(
        eprosima::fastcdr::Cdr& scdr) const
{
    (void) scdr;
}


