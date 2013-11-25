/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "CryptoKeyRSA.h"

#if ENABLE(SUBTLE_CRYPTO)

#include "CryptoAlgorithmDescriptionBuilder.h"
#include "CryptoAlgorithmRegistry.h"
#include "CryptoKeyDataRSAComponents.h"
#include "CryptoKeyPair.h"
#include <CommonCrypto/CommonCryptor.h>

#if defined(__has_include)
#if __has_include(<CommonCrypto/CommonRSACryptor.h>)
#include <CommonCrypto/CommonRSACryptor.h>
#endif
#endif

#ifndef _CC_RSACRYPTOR_H_
enum {
    ccRSAKeyPublic          = 0,
    ccRSAKeyPrivate         = 1
};
typedef uint32_t CCRSAKeyType;
#endif

extern "C" CCCryptorStatus CCRSACryptorCreateFromData(CCRSAKeyType keyType, uint8_t *modulus, size_t modulusLength, uint8_t *exponent, size_t exponentLength, uint8_t *p, size_t pLength, uint8_t *q, size_t qLength, CCRSACryptorRef *ref);
extern "C" CCCryptorStatus CCRSACryptorGeneratePair(size_t keysize, uint32_t e, CCRSACryptorRef *publicKey, CCRSACryptorRef *privateKey);
extern "C" CCRSACryptorRef CCRSACryptorGetPublicKeyFromPrivateKey(CCRSACryptorRef privkey);
extern "C" void CCRSACryptorRelease(CCRSACryptorRef key);
extern "C" CCCryptorStatus CCRSAGetKeyComponents(CCRSACryptorRef rsaKey, uint8_t *modulus, size_t *modulusLength, uint8_t *exponent, size_t *exponentLength, uint8_t *p, size_t *pLength, uint8_t *q, size_t *qLength);
extern "C" CCRSAKeyType CCRSAGetKeyType(CCRSACryptorRef key);

namespace WebCore {

static CCCryptorStatus getPublicKeyComponents(CCRSACryptorRef rsaKey, Vector<uint8_t>& modulus, Vector<uint8_t>& publicExponent)
{
    ASSERT(CCRSAGetKeyType(rsaKey) == ccRSAKeyPublic || CCRSAGetKeyType(rsaKey) == ccRSAKeyPrivate);
    bool keyIsPublic = CCRSAGetKeyType(rsaKey) == ccRSAKeyPublic;
    CCRSACryptorRef publicKey = keyIsPublic ? rsaKey : CCRSACryptorGetPublicKeyFromPrivateKey(rsaKey);

    modulus.resize(16384);
    size_t modulusLength = modulus.size();
    publicExponent.resize(16384);
    size_t exponentLength = publicExponent.size();
    CCCryptorStatus status = CCRSAGetKeyComponents(publicKey, modulus.data(), &modulusLength, publicExponent.data(), &exponentLength, 0, 0, 0, 0);
    if (!keyIsPublic) {
        // CCRSACryptorGetPublicKeyFromPrivateKey has "Get" in the name, but its result needs to be released (see <rdar://problem/15449697>).
        CCRSACryptorRelease(publicKey);
    }
    if (status)
        return status;

    modulus.shrink(modulusLength);
    publicExponent.shrink(exponentLength);
    return status;
}

CryptoKeyRSA::CryptoKeyRSA(CryptoAlgorithmIdentifier identifier, CryptoKeyType type, PlatformRSAKey platformKey, bool extractable, CryptoKeyUsage usage)
    : CryptoKey(identifier, type, extractable, usage)
    , m_platformKey(platformKey)
    , m_restrictedToSpecificHash(false)
{
}

PassRefPtr<CryptoKeyRSA> CryptoKeyRSA::create(CryptoAlgorithmIdentifier identifier, const CryptoKeyDataRSAComponents& keyData, bool extractable, CryptoKeyUsage usage)
{
    if (keyData.type() == CryptoKeyDataRSAComponents::Type::Private && !keyData.hasAdditionalPrivateKeyParameters()) {
        // <rdar://problem/15452324> tracks adding support.
        WTFLogAlways("Private keys without additional data are not supported");
        return nullptr;
    }
    if (keyData.otherPrimeInfos().size()) {
        // <rdar://problem/15444074> tracks adding support.
        WTFLogAlways("Keys with more than two primes are not supported");
        return nullptr;
    }
    CCRSACryptorRef cryptor;
    CCCryptorStatus status = CCRSACryptorCreateFromData(
        keyData.type() == CryptoKeyDataRSAComponents::Type::Public ? ccRSAKeyPublic : ccRSAKeyPrivate,
        (uint8_t*)keyData.modulus().data(), keyData.modulus().size(),
        (uint8_t*)keyData.exponent().data(), keyData.exponent().size(),
        (uint8_t*)keyData.firstPrimeInfo().primeFactor.data(), keyData.firstPrimeInfo().primeFactor.size(),
        (uint8_t*)keyData.secondPrimeInfo().primeFactor.data(), keyData.secondPrimeInfo().primeFactor.size(),
        &cryptor);

    if (status) {
        LOG_ERROR("Couldn't create RSA key from data, error %d", status);
        return nullptr;
    }

    return adoptRef(new CryptoKeyRSA(identifier, keyData.type() == CryptoKeyDataRSAComponents::Type::Public ? CryptoKeyType::Public : CryptoKeyType::Private, cryptor, extractable, usage));
}

CryptoKeyRSA::~CryptoKeyRSA()
{
    CCRSACryptorRelease(m_platformKey);
}

void CryptoKeyRSA::restrictToHash(CryptoAlgorithmIdentifier identifier)
{
    m_restrictedToSpecificHash = true;
    m_hash = identifier;
}

bool CryptoKeyRSA::isRestrictedToHash(CryptoAlgorithmIdentifier& identifier) const
{
    if (!m_restrictedToSpecificHash)
        return false;

    identifier = m_hash;
    return true;
}

size_t CryptoKeyRSA::keySizeInBits() const
{
    Vector<uint8_t> modulus;
    Vector<uint8_t> publicExponent;
    CCCryptorStatus status = getPublicKeyComponents(m_platformKey, modulus, publicExponent);
    if (status) {
        WTFLogAlways("Couldn't get RSA key components, status %d", status);
        return 0;
    }

    return modulus.size() * 8;
}

void CryptoKeyRSA::buildAlgorithmDescription(CryptoAlgorithmDescriptionBuilder& builder) const
{
    CryptoKey::buildAlgorithmDescription(builder);

    Vector<uint8_t> modulus;
    Vector<uint8_t> publicExponent;
    CCCryptorStatus status = getPublicKeyComponents(m_platformKey, modulus, publicExponent);
    if (status) {
        WTFLogAlways("Couldn't get RSA key components, status %d", status);
        return;
    }

    builder.add("modulusLength", modulus.size() * 8);
    builder.add("publicExponent", publicExponent);

    if (m_restrictedToSpecificHash) {
        auto hashDescriptionBuilder = builder.createEmptyClone();
        hashDescriptionBuilder->add("name", CryptoAlgorithmRegistry::shared().nameForIdentifier(m_hash));
        builder.add("hash", *hashDescriptionBuilder);
    }
}

std::unique_ptr<CryptoKeyData> CryptoKeyRSA::exportData() const
{
    ASSERT(extractable());

    switch (CCRSAGetKeyType(m_platformKey)) {
    case ccRSAKeyPublic: {
        Vector<uint8_t> modulus;
        Vector<uint8_t> publicExponent;
        CCCryptorStatus status = getPublicKeyComponents(m_platformKey, modulus, publicExponent);
        if (status) {
            WTFLogAlways("Couldn't get RSA key components, status %d", status);
            return nullptr;
        }
        return CryptoKeyDataRSAComponents::createPublic(modulus, publicExponent);
    }
    case ccRSAKeyPrivate:
        // Not supported yet.
    default:
        return nullptr;
    }
}

static bool bigIntegerToUInt32(const Vector<uint8_t>& bigInteger, uint32_t& result)
{
    result = 0;
    for (size_t i = 0; i + 4 < bigInteger.size(); ++i) {
        if (bigInteger[i])
            return false; // Too big to fit in 32 bits.
    }

    for (size_t i = bigInteger.size() > 4 ? bigInteger.size() - 4 : 0; i < bigInteger.size(); ++i) {
        result <<= 8;
        result += bigInteger[i];
    }
    return true;
}

void CryptoKeyRSA::generatePair(CryptoAlgorithmIdentifier algorithm, unsigned modulusLength, const Vector<uint8_t>& publicExponent, bool extractable, CryptoKeyUsage usage, KeyPairCallback callback, VoidCallback failureCallback)
{
    uint32_t e;
    if (!bigIntegerToUInt32(publicExponent, e)) {
        // Adding support is tracked as <rdar://problem/15444034>.
        WTFLogAlways("Public exponent is too big, not supported");
        failureCallback();
        return;
    }

    // We only use the callback functions when back on the main thread, but captured variables are copied on a secondary thread too.
    KeyPairCallback* localCallback = new KeyPairCallback(std::move(callback));
    VoidCallback* localFailureCallback = new VoidCallback(std::move(failureCallback));

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        CCRSACryptorRef ccPublicKey;
        CCRSACryptorRef ccPrivateKey;
        CCCryptorStatus status = CCRSACryptorGeneratePair(modulusLength, e, &ccPublicKey, &ccPrivateKey);
        if (status) {
            WTFLogAlways("Could not generate a key pair, status %d", status);
            dispatch_async(dispatch_get_main_queue(), ^{
                (*localFailureCallback)();
                delete localFailureCallback;
            });
            return;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            RefPtr<CryptoKeyRSA> publicKey = CryptoKeyRSA::create(algorithm, CryptoKeyType::Public, ccPublicKey, extractable, usage);
            RefPtr<CryptoKeyRSA> privateKey = CryptoKeyRSA::create(algorithm, CryptoKeyType::Private, ccPrivateKey, extractable, usage);
            (*localCallback)(*CryptoKeyPair::create(publicKey.release(), privateKey.release()));
            delete localCallback;
        });
    });
}

} // namespace WebCore

#endif // ENABLE(SUBTLE_CRYPTO)
