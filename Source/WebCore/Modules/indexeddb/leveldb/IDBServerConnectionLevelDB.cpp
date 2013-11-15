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
#include "IDBServerConnectionLevelDB.h"

#if ENABLE(INDEXED_DATABASE)
#if USE(LEVELDB)

#include "IDBBackingStoreLevelDB.h"
#include "IDBBackingStoreTransactionLevelDB.h"
#include "IDBIndexWriter.h"
#include <wtf/MainThread.h>

namespace WebCore {

IDBServerConnectionLevelDB::IDBServerConnectionLevelDB(IDBBackingStoreLevelDB* backingStore)
    : m_backingStore(backingStore)
    , m_closed(false)
{
}

IDBServerConnectionLevelDB::~IDBServerConnectionLevelDB()
{
}

IDBBackingStoreInterface* IDBServerConnectionLevelDB::deprecatedBackingStore()
{
    return m_backingStore.get();
}

IDBBackingStoreTransactionInterface* IDBServerConnectionLevelDB::deprecatedBackingStoreTransaction(int64_t transactionID)
{
    if (!m_backingStore)
        return 0;
    return m_backingStore->deprecatedBackingStoreTransaction(transactionID);
}

bool IDBServerConnectionLevelDB::isClosed()
{
    return m_closed;
}

void IDBServerConnectionLevelDB::getOrEstablishIDBDatabaseMetadata(const String& name, GetIDBDatabaseMetadataFunction callback)
{
    RefPtr<IDBServerConnection> self(this);
    m_backingStore->getOrEstablishIDBDatabaseMetadata(name, [self, this, callback](const IDBDatabaseMetadata& metadata, bool success) {
        callback(metadata, success);
    });
}

void IDBServerConnectionLevelDB::deleteDatabase(const String& name, BoolCallbackFunction successCallback)
{
    RefPtr<IDBServerConnection> self(this);
    m_backingStore->deleteDatabase(name, [self, this, successCallback](bool success) {
        successCallback(success);
    });
}

void IDBServerConnectionLevelDB::close()
{
    m_backingStore.clear();
    m_closed = true;
}

void IDBServerConnectionLevelDB::openTransaction(int64_t transactionID, const HashSet<int64_t>&, IndexedDB::TransactionMode, BoolCallbackFunction successCallback)
{
    if (!m_backingStore) {
        callOnMainThread([successCallback]() {
            successCallback(false);
        });
        return;
    }

    m_backingStore->establishBackingStoreTransaction(transactionID);
    callOnMainThread([successCallback]() {
        successCallback(true);
    });
}

void IDBServerConnectionLevelDB::beginTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->begin();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::commitTransaction(int64_t transactionID, BoolCallbackFunction successCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    bool result = transaction->commit();
    callOnMainThread([successCallback, result]() {
        successCallback(result);
    });
}

void IDBServerConnectionLevelDB::resetTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->resetTransaction();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::rollbackTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->rollback();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::setIndexKeys(int64_t transactionID, int64_t databaseID, int64_t objectStoreID, const IDBObjectStoreMetadata& objectStoreMetadata, IDBKey& primaryKey, const Vector<int64_t>& indexIDs, const Vector<Vector<RefPtr<IDBKey>>>& indexKeys, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> backingStoreTransaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(backingStoreTransaction);

    RefPtr<IDBRecordIdentifier> recordIdentifier;
    bool ok = m_backingStore->keyExistsInObjectStore(*backingStoreTransaction, databaseID, objectStoreID, primaryKey, recordIdentifier);
    if (!ok) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: setting index keys."));
        });
        return;
    }
    if (!recordIdentifier) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: setting index keys for object store."));
        });
        return;
    }

    Vector<RefPtr<IDBIndexWriter>> indexWriters;
    String errorMessage;
    bool obeysConstraints = false;

    bool backingStoreSuccess = m_backingStore->makeIndexWriters(transactionID, databaseID, objectStoreMetadata, primaryKey, false, indexIDs, indexKeys, indexWriters, &errorMessage, obeysConstraints);
    if (!backingStoreSuccess) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: backing store error updating index keys."));
        });
        return;
    }
    if (!obeysConstraints) {
        callOnMainThread([completionCallback, errorMessage]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::ConstraintError, errorMessage));
        });
        return;
    }

    for (size_t i = 0; i < indexWriters.size(); ++i) {
        IDBIndexWriter* indexWriter = indexWriters[i].get();
        indexWriter->writeIndexKeys(recordIdentifier.get(), *m_backingStore, *backingStoreTransaction, databaseID, objectStoreID);
    }

    callOnMainThread([completionCallback]() {
        completionCallback(0);
    });
}


} // namespace WebCore

#endif // USE(LEVELDB)
#endif // ENABLE(INDEXED_DATABASE)
