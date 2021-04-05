// EPOS-- RISC-V 32 MMU Mediator Declarations

#ifndef __riscv32_mmu_h
#define __riscv32_mmu_h

#include <system/memory_map.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/debug.h>
#include <architecture/cpu.h>
#include <architecture/mmu.h>

__BEGIN_SYS

class MMU: public MMU_Common<10, 10, 12>
{
    friend class CPU;
    friend class Setup_SifiveE;

private:
    typedef Grouping_List<Frame> List;

    static const unsigned int PHY_MEM = Memory_Map::PHY_MEM;

public:
    // Page Flags
    class RV32_Flags
    {
        public:
            enum {
                VALID = 1 << 0,
                READ = 1 << 1,
                WRITE = 1 << 2,
                EXEC = 1 << 3,
                ACCESSED = 1 << 6,
                DIRTY = 1 << 7,
                SYS = VALID | READ | WRITE | EXEC,
                USR = VALID | READ | WRITE | EXEC,
            };
        RV32_Flags() {}
        RV32_Flags(const RV32_Flags & f): _flags(f) {}
        RV32_Flags(unsigned int f): _flags(f){}
        RV32_Flags(const Flags & f): _flags(VALID | ((f & Flags::RW) ? READ : 0) | ((f & Flags::USR) ? USR : 0)) {}

        operator unsigned int() const { return _flags; }

        friend Debug & operator<<(Debug & db, const RV32_Flags & f) { db << hex << f._flags; return db; }

    private:
        unsigned int _flags;
    };

    // Page_Table
    class Page_Table
    {

    friend class Setup_SifiveE;
    public:
        Page_Table() {}

        PT_Entry & operator[](unsigned int i) { return _entry[i]; }

        void setflags(const RV32_Flags & flags) {
            for (int i = 0; i < 1024; i++)
            {
                unsigned int pte = (((unsigned)this - Traits<Machine>::PAGE_TABLES)>>12) - 1;
                pte = pte << 20;
                pte += ((i) << 10);
                pte = pte | flags;
                _entry[i] = pte;
            }
        }

        void map(int from, int to, const RV32_Flags & flags) {
            Phy_Addr * addr = alloc(to - from);
            if(addr)
                remap(addr, from, to, flags);
            else
                for( ; from < to; from++) {
                    Log_Addr * tmp = phy2log(&_entry[from]);
                    *tmp = alloc(1) | flags;
                }
        }

        void remap(Phy_Addr addr, int from, int to, const RV32_Flags & flags) {
            addr = align_page(addr);
            for( ; from < to; from++) {
                Log_Addr * tmp = phy2log(&_entry[from]);
                *tmp = addr | flags;
                addr += sizeof(Page);
            }
        }

        void unmap(int from, int to) {
            for( ; from < to; from++) {
                free(_entry[from]);
                Log_Addr * tmp = phy2log(&_entry[from]);
                *tmp = 0;
            }
        }

        friend Debug & operator<<(Debug & db, Page_Table & pt) {
            db << "{\n";
            int brk = 0;
            for(unsigned int i = 0; i < PT_ENTRIES; i++)
                if(pt[i]) {
                    db << "[" << i << "]=" << pt[i] << "  ";
                    if(!(++brk % 4))
                        db << "\n";
                }
            db << "\n}";
            return db;
        }

    private:
        PT_Entry _entry[PT_ENTRIES];
    };

    // Chunk (for Segment)
    class Chunk
    {
    public:
        Chunk() {}
        Chunk(unsigned int bytes, const Flags & flags)
        : _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(RV32_Flags(flags)), _pt(calloc(_pts)) {
            _pt->map(_from, _to, _flags);
        }
        
        Chunk(const Phy_Addr & phy_addr, unsigned int bytes, const Flags & flags)
        : _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(RV32_Flags(flags)), _pt(calloc(_pts)) {
            _pt->remap(phy_addr, _from, _to, flags);
        }

        ~Chunk() {
            for( ; _from < _to; _from++) {
                free((*static_cast<Page_Table *>(phy2log(_pt)))[_from]);
            }
            free(_pt, _pts);
        }

        unsigned int pts() const { return _pts; }
        RV32_Flags flags() const { return _flags; }
        Page_Table * pt() const { return _pt; }
        unsigned int size() const { return (_to - _from) * sizeof(Page); }

        Phy_Addr phy_address() const { return Phy_Addr(false); }

        int resize(unsigned int amount) {
            unsigned int pgs = pages(amount);
            unsigned int free_pgs = _pts * PT_ENTRIES - _to;
            if(free_pgs < pgs) { // resize _pt
                unsigned int pts = _pts + page_tables(pgs - free_pgs);
                Page_Table * pt = calloc(pts);
                memcpy(pt, _pt, _pts * sizeof(Page));
                free(_pt, _pts);
                _pt = pt;
                _pts = pts;
            }

            _pt->map(_to, _to + pgs, _flags);
            _to += pgs;

            return pgs * sizeof(Page);
        }

    private:
        unsigned int _from;
        unsigned int _to;
        unsigned int _pts;
        RV32_Flags _flags;
        Page_Table * _pt;
    };

    // Page Directory
    typedef Page_Table Page_Directory;

    // Directory (for Address_Space)
    class Directory
    {
    public:
        Directory() : _pd(calloc(1)), _free(true) {
            for(unsigned int i = directory(PHY_MEM); i < PD_ENTRIES; i++)
                (*_pd)[i] = (*_master)[i];
        }

        Directory(Page_Directory * pd) : _pd(pd), _free(false) {}

        ~Directory() { if(_free) free(_pd); }

        Phy_Addr pd() const { return _pd; }

        void activate() const { CPU::pdp(reinterpret_cast<CPU::Reg32>(_pd)); }

        Log_Addr attach(const Chunk & chunk, unsigned int from = 0) {
            for(unsigned int i = from; i < PD_ENTRIES; i++)
                if(attach(i, chunk.pt(), chunk.pts(), chunk.flags()))
                    return i << DIRECTORY_SHIFT;
            return false;
        }

        Log_Addr attach(const Chunk & chunk, const Log_Addr & addr) {
            unsigned int from = directory(addr);
            if(!attach(from, chunk.pt(), chunk.pts(), chunk.flags()))
                return Log_Addr(false);
            return from << DIRECTORY_SHIFT;
        }

        void detach(const Chunk & chunk) {
            for(unsigned int i = 0; i < PD_ENTRIES; i++)
                if(indexes((*_pd)[i]) == indexes(chunk.pt())) {
                    detach(i, chunk.pt(), chunk.pts());
                return;
            }
            db<MMU>(WRN) << "MMU::Directory::detach(pt=" << chunk.pt() << ") failed!" << endl;
        }

        void detach(const Chunk & chunk, const Log_Addr & addr) {
            unsigned int from = directory(addr);
            if(indexes((*static_cast<Log_Addr *>(phy2log(_pd)))[from]) != indexes(chunk.pt())) {
                db<MMU>(WRN) << "MMU::Directory::detach(pt=" << chunk.pt() << ",addr=" << addr << ") failed!" << endl;
                return;
            }
            detach(from, chunk.pt(), chunk.pts());
        }

        Phy_Addr physical(const Log_Addr & addr) {
            Page_Table * pt = reinterpret_cast<Page_Table *>((void *)(*_pd)[directory(addr)]);
            return (*pt)[page(addr)] | offset(addr);
        }

    private:
        bool attach(unsigned int from, const Page_Table * pt, unsigned int n, RV32_Flags flags) {
            for(unsigned int i = from; i < from + n; i++)
                if((*static_cast<Page_Directory *>(phy2log(_pd)))[i])
                    return false;
            for(unsigned int i = from; i < from + n; i++, pt++)
                (*static_cast<Page_Directory *>(phy2log(_pd)))[i] = Phy_Addr(pt) | flags;
            return true;
        }

        void detach(unsigned int from, const Page_Table * pt, unsigned int n) {
            for(unsigned int i = from; i < from + n; i++)
                (*static_cast<Page_Directory *>(phy2log(_pd)))[i] = 0;
        }

    private:
        Page_Directory * _pd;
        bool _free;
    };

    // DMA_Buffer (straightforward without paging)
    class DMA_Buffer: public Chunk
    {
    public:
        DMA_Buffer(unsigned int s): Chunk(s, Flags::CT) {
            db<MMU>(TRC) << "MMU::DMA_Buffer() => " << *this << endl;
        }

        Log_Addr log_address() const { return phy_address(); }

        friend Debug & operator<<(Debug & db, const DMA_Buffer & b) {
            db << "{phy=" << b.phy_address()
               << ",log=" << b.log_address()
               << ",size=" << b.size()
               << ",flags=" << b.flags() << "}";
            return db;
        }
    };

public:
    MMU() {}

    static Phy_Addr alloc(unsigned int frames = 1) {
        Phy_Addr phy(false);
        if(frames) {
            List::Element * e = _free.search_decrementing(frames);
            if(e) {
                phy = e->object() + e->size();
                db<MMU>(TRC) << "MMU::alloc(frames=" << frames << ") => " << phy << endl;
            } else
                db<MMU>(WRN) << "MMU::alloc(frames=" << frames << ") => failed!" << endl;
        }

        return phy;
    };

    static Phy_Addr calloc(unsigned int frames = 1) {
        Phy_Addr phy = alloc(frames);
        memset(phy2log(phy), 0, sizeof(Frame) * frames);
        return phy;
    }

    static void free(Phy_Addr frame, int n = 1) {
        // Clean up MMU flags in frame address
        frame = indexes(frame);
        db<MMU>(TRC) << "MMU::free(frame=" << frame << ",n=" << n << ")" << endl;

        if(frame && n) {
            List::Element * e = new (phy2log(frame)) List::Element(frame, n);
            List::Element * m1, * m2;
            _free.insert_merging(e, &m1, &m2);
        }
    }

    static unsigned int allocable() { return _free.head() ? _free.head()->size() : 0; }

    static Page_Directory * volatile current() {
        return 0;
        //return reinterpret_cast<Page_Directory * volatile>(CPU::pdp());
    }

    static Phy_Addr physical(const Log_Addr & addr) {
        Page_Directory * pd = current();
        Page_Table * pt = (*pd)[directory(addr)];
        return (*pt)[page(addr)] | offset(addr);
    }

    /*static void flush_tlb() {
        ASM("movl %cr3,%eax");  
        ASM("movl %eax,%cr3");
    }
    static void flush_tlb(const Log_Addr & addr) {
        ASM("invlpg %0" : : "m"(addr));
    }*/

private:
    static void init();
    static Log_Addr phy2log(const Phy_Addr & phy) { return phy | PHY_MEM; }

private:
    static List _free;
    static Page_Directory * _master;
};

__END_SYS

#endif