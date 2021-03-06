/**
 * PowerDNS Authoritative HTTP API
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * The version of the OpenAPI document: 0.0.13
 *
 * NOTE: This class is auto generated by OpenAPI-Generator 4.2.1.
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */



#include "SearchResults.h"

namespace org {
namespace openapitools {
namespace client {
namespace model {




SearchResults::SearchResults()
{
    m_DataIsSet = false;
}

SearchResults::~SearchResults()
{
}

void SearchResults::validate()
{
    // TODO: implement validation
}

web::json::value SearchResults::toJson() const
{
    web::json::value val = web::json::value::object();

    {
        std::vector<web::json::value> jsonArray;
        for( auto& item : m_Data )
        {
            jsonArray.push_back(ModelBase::toJson(item));
        }
        if(jsonArray.size() > 0)
        {
            val[utility::conversions::to_string_t("data")] = web::json::value::array(jsonArray);
        }
    }

    return val;
}

void SearchResults::fromJson(const web::json::value& val)
{
    {
        m_Data.clear();
        std::vector<web::json::value> jsonArray;
        if(val.has_field(utility::conversions::to_string_t("data")))
        {
        for( auto& item : val.at(utility::conversions::to_string_t("data")).as_array() )
        {
            if(item.is_null())
            {
                m_Data.push_back( std::shared_ptr<SearchResult>(nullptr) );
            }
            else
            {
                std::shared_ptr<SearchResult> newItem(new SearchResult());
                newItem->fromJson(item);
                m_Data.push_back( newItem );
            }
        }
        }
    }
}

void SearchResults::toMultipart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix) const
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    {
        std::vector<web::json::value> jsonArray;
        for( auto& item : m_Data )
        {
            jsonArray.push_back(ModelBase::toJson(item));
        }
        
        if(jsonArray.size() > 0)
        {
            multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("data"), web::json::value::array(jsonArray), utility::conversions::to_string_t("application/json")));
        }
    }
}

void SearchResults::fromMultiPart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix)
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    {
        m_Data.clear();
        if(multipart->hasContent(utility::conversions::to_string_t("data")))
        {

        web::json::value jsonArray = web::json::value::parse(ModelBase::stringFromHttpContent(multipart->getContent(utility::conversions::to_string_t("data"))));
        for( auto& item : jsonArray.as_array() )
        {
            if(item.is_null())
            {
                m_Data.push_back( std::shared_ptr<SearchResult>(nullptr) );
            }
            else
            {
                std::shared_ptr<SearchResult> newItem(new SearchResult());
                newItem->fromJson(item);
                m_Data.push_back( newItem );
            }
        }
        }
    }
}

std::vector<std::shared_ptr<SearchResult>>& SearchResults::getData()
{
    return m_Data;
}

void SearchResults::setData(const std::vector<std::shared_ptr<SearchResult>>& value)
{
    m_Data = value;
    m_DataIsSet = true;
}

bool SearchResults::dataIsSet() const
{
    return m_DataIsSet;
}

void SearchResults::unsetData()
{
    m_DataIsSet = false;
}

}
}
}
}


