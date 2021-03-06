/* This file is part of VoltDB.
 * Copyright (C) 2008-2014 VoltDB Inc.
 *
 * This file contains original code and/or modifications of original code.
 * Any modifications made by VoltDB Inc. are licensed under the following
 * terms and conditions:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Copyright (C) 2008 by H-Store Project
 * Brown University
 * Massachusetts Institute of Technology
 * Yale University
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HSTORETABLE_H
#define HSTORETABLE_H
#ifndef BTREE_DEBUG
#define BTREE_DEBUG
#endif
#include <string>
#include <vector>
#include <set>
#include <list>
#include <cassert>

#include "common/ids.h"
#include "common/types.h"
#include "common/TupleSchema.h"
#include "common/Pool.hpp"
#include "common/tabletuple.h"
#include "common/TheHashinator.h"
#include "storage/TupleBlock.h"
#include "stx/btree_set.h"
#include "common/ThreadLocalPool.h"

namespace voltdb {

class TableIndex;
class TableColumn;
class TableTuple;
class TableFactory;
class TableIterator;
class CopyOnWriteIterator;
class CopyOnWriteContext;
class UndoLog;
class ReadWriteSet;
class SerializeInput;
class SerializeOutput;
class TableStats;
class StatsSource;
class StreamBlock;
class Topend;
class TupleBlock;
class PersistentTableUndoDeleteAction;

const size_t COLUMN_DESCRIPTOR_SIZE = 1 + 4 + 4; // type, name offset, name length

/**
 * Represents a table which might or might not be a temporary table.
 * All tables, TempTable, PersistentTable and StreamedTable are derived
 * from this class.
 *
 * Table objects including derived classes are only instantiated via a
 * factory class (TableFactory).
 */
class Table {
    friend class TableFactory;
    friend class TableIterator;
    friend class CopyOnWriteContext;
    friend class ExecutionEngine;
    friend class TableStats;
    friend class StatsSource;
    friend class TupleBlock;
    friend class PersistentTableUndoDeleteAction;

  private:
    Table();
    Table(Table const&);

  public:
    virtual ~Table();

    /*
     * Table lifespan can be managed bya reference count. The
     * reference is trivial to maintain since it is only accessed by
     * the execution engine thread. Snapshot, Export and the
     * corresponding CatalogDelegate may be reference count
     * holders. The table is deleted when the refcount falls to
     * zero. This allows longer running processes to complete
     * gracefully after a table has been removed from the catalog.
     */
    void incrementRefcount() {
        m_refcount += 1;
    }

    void decrementRefcount() {
        m_refcount -= 1;
        if (m_refcount == 0) {
            delete this;
        }
    }

    // ------------------------------------------------------------------
    // ACCESS METHODS
    // ------------------------------------------------------------------
    virtual TableIterator& iterator() = 0;
    virtual TableIterator *makeIterator() = 0;

    // ------------------------------------------------------------------
    // OPERATIONS
    // ------------------------------------------------------------------
    virtual void deleteAllTuples(bool freeAllocatedStrings) = 0;
    // TODO: change meaningless bool return type to void (starting in class Table) and migrate callers.
    // The fallible flag is used to denote a change to a persistent table
    // which is part of a long transaction that has been vetted and can
    // never fail (e.g. violate a constraint).
    // The initial use case is a live catalog update that changes table schema and migrates tuples
    // and/or adds a materialized view.
    // Constraint checks are bypassed and the change does not make use of "undo" support.
    virtual bool deleteTuple(TableTuple &tuple, bool fallible=true) = 0;
    // TODO: change meaningless bool return type to void (starting in class Table) and migrate callers.
    // -- Most callers should be using TempTable::insertTempTuple, anyway.
    virtual bool insertTuple(TableTuple &tuple) = 0;

    // ------------------------------------------------------------------
    // TUPLES AND MEMORY USAGE
    // ------------------------------------------------------------------
    virtual size_t allocatedBlockCount() const = 0;

    TableTuple& tempTuple() {
        assert (m_tempTuple.m_data);
        return m_tempTuple;
    }

    int64_t allocatedTupleCount() const {
        return allocatedBlockCount() * m_tuplesPerBlock;
    }

    /**
     * Includes tuples that are pending any kind of delete.
     * Used by iterators to determine how many tupels to expect while scanning
     */
    virtual int64_t activeTupleCount() const {
        return m_tupleCount;
    }

    virtual int64_t allocatedTupleMemory() const {
        return allocatedBlockCount() * m_tableAllocationSize;
    }

    int64_t occupiedTupleMemory() const {
        return m_tupleCount * m_tempTuple.tupleLength();
    }

    // Only counts persistent table usage, currently
    int64_t nonInlinedMemorySize() const {
        return m_nonInlinedMemorySize;
    }

    // ------------------------------------------------------------------
    // COLUMNS
    // ------------------------------------------------------------------
    int columnIndex(const std::string &name) const;
    const std::vector<std::string>& getColumnNames() const {
        return m_columnNames;
    }


    inline const TupleSchema* schema() const {
        return m_schema;
    }

    inline const std::string& columnName(int index) const {
        return m_columnNames[index];
    }

    inline int columnCount() const {
        return m_columnCount;
    }

    // ------------------------------------------------------------------
    // INDEXES
    // ------------------------------------------------------------------
    virtual int indexCount() const {
        return static_cast<int>(m_indexes.size());
    }

    virtual int uniqueIndexCount() const {
        return static_cast<int>(m_uniqueIndexes.size());
    }

    // returned via shallow vector copy -- seems good enough.
    const std::vector<TableIndex*>& allIndexes() const { return m_indexes; }

    virtual TableIndex *index(std::string name);

    virtual TableIndex *primaryKeyIndex() {
        return m_pkeyIndex;
    }
    virtual const TableIndex *primaryKeyIndex() const {
        return m_pkeyIndex;
    }

    void configureIndexStats(CatalogId databaseId);

    // mutating indexes
    virtual void addIndex(TableIndex *index);
    virtual void removeIndex(TableIndex *index);
    virtual void setPrimaryKeyIndex(TableIndex *index);

    // ------------------------------------------------------------------
    // UTILITY
    // ------------------------------------------------------------------
    const std::string& name() const {
        return m_name;
    }

    CatalogId databaseId() const {
        return m_databaseId;
    }

    virtual std::string tableType() const = 0;
    virtual std::string debug();

    // ------------------------------------------------------------------
    // SERIALIZATION
    // ------------------------------------------------------------------
    int getApproximateSizeToSerialize() const;
    bool serializeTo(SerializeOutput &serialize_out);
    bool serializeColumnHeaderTo(SerializeOutput &serialize_io);

    /*
     * Serialize a single tuple as a table so it can be sent to Java.
     */
    bool serializeTupleTo(SerializeOutput &serialize_out, TableTuple *tuples, int numTuples);

    /**
     * Loads only tuple data and assumes there is no schema present.
     * Used for recovery where the schema is not sent.
     */
    void loadTuplesFromNoHeader(SerializeInput &serialize_in,
                                Pool *stringPool = NULL,
                                ReferenceSerializeOutput *uniqueViolationOutput = NULL);

    /**
     * Loads only tuple data, not schema, from the serialized table.
     * Used for initial data loading and receiving dependencies.
     */
    void loadTuplesFrom(SerializeInput &serialize_in,
                        Pool *stringPool = NULL,
                        ReferenceSerializeOutput *uniqueViolationOutput = NULL);


    // ------------------------------------------------------------------
    // EXPORT
    // ------------------------------------------------------------------

    /**
     * Set the current offset in bytes of the export stream for this Table
     * since startup (used for rejoin/recovery).
     */
    virtual void setExportStreamPositions(int64_t seqNo, size_t streamBytesUsed) {
        // this should be overidden by any table involved in an export
        assert(false);
    }

    /**
     * Get the current offset in bytes of the export stream for this Table
     * since startup (used for rejoin/recovery).
     */
    virtual void getExportStreamPositions(int64_t &seqNo, size_t &streamBytesUsed) {
        // this should be overidden by any table involved in an export
        assert(false);
    }

    /**
     * Release any committed Export bytes up to the provided stream offset
     */
    virtual bool releaseExportBytes(int64_t releaseOffset) {
        // default implementation returns false, which
        // indicates an error
        return false;
    }

    /**
     * Reset the Export poll marker
     */
    virtual void resetPollMarker() {
        // default, do nothing.
    }

    /**
     * Flush tuple stream wrappers. A negative time instructs an
     * immediate flush.
     */
    virtual void flushOldTuples(int64_t timeInMillis) {
    }
    /**
     * Inform the tuple stream wrapper of the table's signature and the timestamp
     * of the current export generation
     */
    virtual void setSignatureAndGeneration(std::string signature, int64_t generation) {
    }

    virtual bool isExport() {
        return false;
    }

    /**
     * These metrics are needed by some iterators.
     */
    uint32_t getTupleLength() const {
        return m_tupleLength;
    }
    int getTableAllocationSize() const {
        return m_tableAllocationSize;
    }
    uint32_t getTuplesPerBlock() const {
        return m_tuplesPerBlock;
    }

    virtual int64_t validatePartitioning(TheHashinator *hashinator, int32_t partitionId) {
        throwFatalException("Validate partitioning unsupported on this table type");
        return 0;
    }

protected:
    /*
     * Implemented by persistent table and called by Table::loadTuplesFrom
     * to do additional processing for views and Export
     */
    virtual void processLoadedTuple(TableTuple &tuple,
                                    ReferenceSerializeOutput *uniqueViolationOutput,
                                    int32_t &serializedTupleCount,
                                    size_t &tupleCountPosition) {
    };

    virtual void swapTuples(TableTuple &sourceTupleWithNewValues, TableTuple &destinationTuple) {
        throwFatalException("Unsupported operation");
    }

public:

    virtual bool equals(voltdb::Table *other);
    virtual voltdb::TableStats* getTableStats() = 0;

protected:
    // virtual block management functions
    virtual void nextFreeTuple(TableTuple *tuple) = 0;

    Table(int tableAllocationTargetSize);
    void resetTable();

    bool compactionPredicate() {
        //Unfortunate work around for the fact that multiple undo quantums cause this to happen
        //Ideally there would be one per transaction and we could hard fail or
        //the undo log would only trigger compaction once per transaction
        if (m_tuplesPinnedByUndo != 0) {
            return false;
        }
        return allocatedTupleCount() - activeTupleCount() > (m_tuplesPerBlock * 3) && loadFactor() < .95;
    }

    void initializeWithColumns(TupleSchema *schema, const std::vector<std::string> &columnNames, bool ownsTupleSchema);

    // per table-type initialization
    virtual void onSetColumns() {
    };

    double loadFactor() {
        return static_cast<double>(activeTupleCount()) /
            static_cast<double>(allocatedTupleCount());
    }


    // ------------------------------------------------------------------
    // DATA
    // ------------------------------------------------------------------

  protected:
    TableTuple m_tempTuple;
    boost::scoped_array<char> m_tempTupleMemory;

    TupleSchema* m_schema;

    // schema as array of string names
    std::vector<std::string> m_columnNames;
    char *m_columnHeaderData;
    int32_t m_columnHeaderSize;

    uint32_t m_tupleCount;
    uint32_t m_tuplesPinnedByUndo;
    uint32_t m_columnCount;
    uint32_t m_tuplesPerBlock;
    uint32_t m_tupleLength;
    int64_t m_nonInlinedMemorySize;

    // identity information
    CatalogId m_databaseId;
    std::string m_name;

    // If this table owns the TupleSchema it is responsible for deleting it in the destructor
    bool m_ownsTupleSchema;

    const int m_tableAllocationTargetSize;
    int m_tableAllocationSize;

    // indexes
    std::vector<TableIndex*> m_indexes;
    std::vector<TableIndex*> m_uniqueIndexes;
    TableIndex *m_pkeyIndex;

  private:
    int32_t m_refcount;
    ThreadLocalPool m_tlPool;
};

}
#endif
