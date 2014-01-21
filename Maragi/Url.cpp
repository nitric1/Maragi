#include "Common.h"

#include "Batang/Utility.h"

#include "Url.h"

namespace Maragi
{
    Url::Url()
        : changed_(true)
    {
    }

    Url::Url(const std::string &baseUrl, const std::map<std::string, std::string> &params,
        const std::map<std::string, std::string> &oauthParams)
        : changed_(true), baseUrl_(baseUrl), params_(params), oauthParams_(oauthParams)
    {
    }

    void Url::assign(const std::string &baseUrl, const std::map<std::string, std::string> &params,
        const std::map<std::string, std::string> &oauthParams)
    {
        baseUrl_ = baseUrl;
        params_ = params;
        oauthParams_ = oauthParams;
        changed_ = true;
    }

    bool Url::hasParam(const std::string &key) const
    {
        return params_.find(key) != params_.end();
    }

    const std::string &Url::getParam(const std::string &key) const
    {
        static std::string empty;
        auto it = params_.find(key);
        if(it != params_.end())
            return it->second;
        return empty;
    }

    bool Url::addParam(const std::string &key, const std::string &value)
    {
        if(params_.insert(make_pair(key, value)).second)
        {
            changed_ = true;
            return true;
        }
        return false;
    }

    void Url::removeParam(const std::string &key)
    {
        params_.erase(key);
        changed_ = true;
    }

    bool Url::hasOAuthParam(const std::string &key) const
    {
        return oauthParams_.find(key) != oauthParams_.end();
    }

    const std::string &Url::getOAuthParam(const std::string &key) const
    {
        static std::string empty;
        auto it = oauthParams_.find(key);
        if(it != oauthParams_.end())
            return it->second;
        return empty;
    }

    bool Url::addOAuthParam(const std::string &key, const std::string &value)
    {
        if(oauthParams_.insert(make_pair(key, value)).second)
        {
            changed_ = true;
            return true;
        }
        return false;
    }

    void Url::removeOAuthParam(const std::string &key)
    {
        oauthParams_.erase(key);
        changed_ = true;
    }

    const std::string &Url::baseUrl() const
    {
        return baseUrl_;
    }

    void Url::baseUrl(const std::string &baseUrl)
    {
        baseUrl_ = baseUrl;
        changed_ = true;
    }

    const std::map<std::string, std::string> &Url::params() const
    {
        return params_;
    }

    void Url::params(const std::map<std::string, std::string> &params)
    {
        params_ = params;
        changed_ = true;
    }

    const std::map<std::string, std::string> &Url::oauthParams() const
    {
        return oauthParams_;
    }

    void Url::oauthParams(const std::map<std::string, std::string> &oauthParams)
    {
        oauthParams_ = oauthParams;
        changed_ = true;
    }

    std::string Url::compose() const
    {
        if(changed_)
        {
            composedUrl_ = baseUrl_;
            if(!params_.empty())
            {
                auto it = params_.begin();
                composedUrl_ += "?";
                composedUrl_ += Batang::encodeUrlParam(it->first);
                composedUrl_ += "=";
                composedUrl_ += Batang::encodeUrlParam(it->second);
                for(++ it; it != params_.end(); ++ it)
                {
                    composedUrl_ += "&";
                    composedUrl_ += Batang::encodeUrlParam(it->first);
                    composedUrl_ += "=";
                    composedUrl_ += Batang::encodeUrlParam(it->second);
                }
            }
        }

        return composedUrl_;
    }

    Url::operator std::string() const
    {
        return compose();
    }
}
