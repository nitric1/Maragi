#pragma once

namespace Maragi
{
    class Url
    {
    private:
        std::string baseUrl_;
        std::map<std::string, std::string> params_;
        std::map<std::string, std::string> oauthParams_;
        mutable bool changed_;
        mutable std::string composedUrl_;

    public:
        Url();
        Url(
            const std::string &,
            const std::map<std::string, std::string> & = std::map<std::string, std::string>(),
            const std::map<std::string, std::string> & = std::map<std::string, std::string>()
        );
        Url(const Url &) = default;
        Url(Url &&) = default;

    public:
        void assign(
            const std::string &,
            const std::map<std::string, std::string> & = std::map<std::string, std::string>(),
            const std::map<std::string, std::string> & = std::map<std::string, std::string>()
        );

        bool hasParam(const std::string &) const;
        const std::string &getParam(const std::string &) const;
        bool addParam(const std::string &, const std::string &);
        void removeParam(const std::string &);

        bool hasOAuthParam(const std::string &) const;
        const std::string &getOAuthParam(const std::string &) const;
        bool addOAuthParam(const std::string &, const std::string &);
        void removeOAuthParam(const std::string &);

        const std::string &baseUrl() const;
        void baseUrl(const std::string &);
        const std::map<std::string, std::string> &params() const;
        void params(const std::map<std::string, std::string> &);
        const std::map<std::string, std::string> &oauthParams() const;
        void oauthParams(const std::map<std::string, std::string> &);
        std::string compose() const;

    public:
        Url &operator =(const Url &) = default;
        Url &operator =(Url &&) = default;
        operator std::string() const;
    };
}
