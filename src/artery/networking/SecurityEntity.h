#ifndef ARTERY_SECURITYENTITY_H_UWBA0SPJ
#define ARTERY_SECURITYENTITY_H_UWBA0SPJ

#include <omnetpp/csimplemodule.h>
#include <vanetza/security/backend.hpp>
#include <vanetza/security/certificate_cache.hpp>
#include <vanetza/security/certificate_provider.hpp>
#include <vanetza/security/certificate_validator.hpp>
#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/sign_header_policy.hpp>
#include <vanetza/security/sign_service.hpp>
#include <vanetza/security/verify_service.hpp>
#include <memory>
#include <string>

namespace vanetza {
    class PositionProvider;
    class Runtime;
} // namespace vanetza

namespace artery
{

class SecurityEntity : public omnetpp::cSimpleModule, public vanetza::security::SecurityEntity
{
    public:
        // cSimpleModule
        int numInitStages() const override;
        void initialize(int stage) override;
        void finish() override;

        // SecurityEntity
        vanetza::security::EncapConfirm encapsulate_packet(vanetza::security::EncapRequest&&) override;
        vanetza::security::DecapConfirm decapsulate_packet(vanetza::security::DecapRequest&&) override;

    protected:
        std::unique_ptr<vanetza::security::Backend> createBackend(const std::string&) const;
        std::unique_ptr<vanetza::security::CertificateProvider> createCertificateProvider(const std::string&) const;
        std::unique_ptr<vanetza::security::CertificateValidator> createCertificateValidator(const std::string&) const;
        vanetza::security::SignService createSignService(const std::string&) const;
        vanetza::security::VerifyService createVerifyService(const std::string&) const;

    private:
        vanetza::Runtime* mRuntime;
        vanetza::PositionProvider* mPositionProvider;
        std::unique_ptr<vanetza::security::Backend> mBackend;
        std::unique_ptr<vanetza::security::CertificateProvider> mCertificateProvider;
        std::unique_ptr<vanetza::security::CertificateValidator> mCertificateValidator;
        std::unique_ptr<vanetza::security::CertificateCache> mCertificateCache;
        std::unique_ptr<vanetza::security::SignHeaderPolicy> mSignHeaderPolicy;
        std::unique_ptr<vanetza::security::SecurityEntity> mEntity;
};

} // namespace artery

#endif /* ARTERY_SECURITYENTITY_H_UWBA0SPJ */

