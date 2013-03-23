#include "Common.h"

#include "URI.h"

#include "Utility.h"

namespace Maragi
{
    URI::URI()
        : changed(true)
    {
    }

    URI::URI(const std::string &ibaseURI, const std::map<std::string, std::string> &iparams,
        const std::map<std::string, std::string> &ioauthParams)
        : changed(true), baseURI(ibaseURI), params(iparams), oauthParams(ioauthParams)
    {
    }

    URI::URI(const URI &uri)
        : changed(uri.changed), baseURI(uri.baseURI), params(uri.params), oauthParams(uri.oauthParams)
    {
    }

    URI::URI(URI &&uri)
        : changed(uri.changed), baseURI(std::move(uri.baseURI)), params(std::move(uri.params))
        , oauthParams(std::move(uri.oauthParams))
    {
    }

    void URI::assign(const std::string &ibaseURI, const std::map<std::string, std::string> &iparams,
        const std::map<std::string, std::string> &ioauthParams)
    {
        baseURI = ibaseURI;
        params = iparams;
        oauthParams = ioauthParams;
        changed = true;
    }

    void URI::assignBaseURI(const std::string &ibaseURI)
    {
        baseURI = ibaseURI;
        changed = true;
    }

    void URI::assignParam(const std::map<std::string, std::string> &iparams)
    {
        params = iparams;
        changed = true;
    }

    void URI::assignOAuthParam(const std::map<std::string, std::string> &ioauthParams)
    {
        oauthParams = ioauthParams;
        changed = true;
    }

    bool URI::hasParam(const std::string &key) const
    {
        return params.find(key) != params.end();
    }

    const std::string &URI::getParam(const std::string &key) const
    {
        static std::string empty;
        auto it = params.find(key);
        if(it != params.end())
            return it->second;
        return empty;
    }

    bool URI::addParam(const std::string &key, const std::string &value)
    {
        if(params.insert(make_pair(key, value)).second)
        {
            changed = true;
            return true;
        }
        return false;
    }

    void URI::removeParam(const std::string &key)
    {
        params.erase(key);
        changed = true;
    }

    bool URI::hasOAuthParam(const std::string &key) const
    {
        return oauthParams.find(key) != oauthParams.end();
    }

    const std::string &URI::getOAuthParam(const std::string &key) const
    {
        static std::string empty;
        auto it = oauthParams.find(key);
        if(it != oauthParams.end())
            return it->second;
        return empty;
    }

    bool URI::addOAuthParam(const std::string &key, const std::string &value)
    {
        if(oauthParams.insert(make_pair(key, value)).second)
        {
            changed = true;
            return true;
        }
        return false;
    }

    void URI::removeOAuthParam(const std::string &key)
    {
        oauthParams.erase(key);
        changed = true;
    }

    const std::string &URI::getBaseURI() const
    {
        return baseURI;
    }

    const std::map<std::string, std::string> &URI::getParams() const
    {
        return params;
    }

    const std::map<std::string, std::string> &URI::getOAuthParams() const
    {
        return oauthParams;
    }

    std::string URI::getStringURI() const
    {
        if(changed)
        {
            uriString = baseURI;
            if(!params.empty())
            {
                auto it = params.begin();
                uriString += "?";
                uriString += encodeURIParam(it->first);
                uriString += "=";
                uriString += encodeURIParam(it->second);
                for(++ it; it != params.end(); ++ it)
                {
                    uriString += "&";
                    uriString += encodeURIParam(it->first);
                    uriString += "=";
                    uriString += encodeURIParam(it->second);
                }
            }
        }

        return uriString;
    }

    URI &URI::operator =(const URI &uri)
    {
        if(this != &uri)
        {
            baseURI = uri.baseURI;
            params = uri.params;
            oauthParams = uri.oauthParams;
            changed = uri.changed;
            uriString = uri.uriString;
        }

        return *this;
    }

    URI &URI::operator =(URI &&uri)
    {
        if(this != &uri)
        {
            baseURI = std::move(uri.baseURI);
            params = std::move(uri.params);
            oauthParams = std::move(uri.oauthParams);
            changed = uri.changed;
            uriString = std::move(uri.uriString);
        }

        return *this;
    }

    URI::operator std::string() const
    {
        return getStringURI();
    }
}
