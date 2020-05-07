/*
 * GEL++ ELF DebugLine class interface
 * Copyright (c) 2020, IRIT- université de Toulouse
 *
 * GEL++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GEL++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GEL++; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GELPP_ELF_DEBUG_LINE_H
#define GELPP_ELF_DEBUG_LINE_H

#include <elm/data/FragTable.h>
#include <elm/data/HashMap.h>
#include <gel++/elf/defs.h>
#include <gel++/elf/File.h>
#include <gel++.h>
#include <gel++/elf/ArchPlugin.h>

namespace gel { namespace elf {

using namespace elm;

class DebugLine {
public:

	class CompilationUnit;

	class File {
		friend class DebugLine;
	public:
		inline File(): _date(0), _size(0) { }
		inline File(sys::Path path, t::uint64 date = 0, size_t size = 0):
			_path(path), _date(date), _size(size) { }

		inline const sys::Path& path() const { return _path; }
		inline t::uint64 date() const { return _date; }
		inline size_t size() const { return _size; }
		inline const List<CompilationUnit *>& units() const { return _units; }
		void find(int line, Vector<Pair<address_t, address_t> >& addrs) const;

	private:
		sys::Path _path;
		t::uint64 _date;
		size_t _size;
		List<CompilationUnit *> _units;
	};

	class Line {
	public:
		static const t::uint32
			IS_STMT			= 1 << 0,
			BASIC_BLOCK		= 1 << 1,
			PROLOGUE_END	= 1 << 2,
			EPILOGUE_BEGIN	= 1 << 3;

		inline Line()
			: _file(nullptr), _line(0), _col(0), _flags(0), _addr(0), _isa(0),
			  _disc(0), _opi(0) { }

		Line(address_t addr, File *file, int line, int col = 0,
		t::uint32 flags = 0, t::uint8 isa = 0, t::uint8 desc = 0, t::uint8 opi = 0);

		inline File *file() const { return _file; }
		inline int line() const { return _line; }
		inline int col() const { return _col; }
		inline t::uint32 flags() const { return _flags; }
		inline address_t addr() const { return _addr; }
		inline t::uint8 isa() const { return _isa; }
		inline t::uint8 discriminator() const { return _disc; }
		inline t::uint8 op_index() const { return _opi; }

	private:
		File *_file;
		int _line, _col;
		t::uint32 _flags;
		address_t _addr;
		t::uint8 _isa, _disc, _opi;
	};

	class CompilationUnit {
		friend class DebugLine;
	public:
		const FragTable<Line>& lines() const { return _lines; }
		const Vector<File *>& files() const { return _files; }
	private:
		Vector<File *> _files;
		FragTable<Line> _lines;
	};

	class StateMachine {
	public:
		inline StateMachine() { include_directories.add("."); }
		address_t address = 0;
		t::uint32
			op_index = 0,
			file = 1,
			line = 1,
			column = 0,
			isa = 0,
			discriminator = 0;
		bool end_sequence = false;
		t::uint8 flags = 0;
		inline void set(t::uint8 m) { flags |= m; }
		inline void clear(t::uint8 m) { flags &= ~m; }
		inline bool bit(t::uint8 m) { return (flags & m) != 0; }
		t::int8 line_base = 0;
		t::uint8
			line_range = 0,
			opcode_base = 0;
		const t::uint8 *standard_opcode_lengths = nullptr;
		t::uint8
			minimum_instruction_length = 0,
			maximum_operations_per_instruction = 0;
		Vector<cstring> include_directories;
	};

	DebugLine(elf::File *efile);
	~DebugLine();

	inline const HashMap<sys::Path, File *>& files() const { return _files; }
	inline const FragTable<CompilationUnit *>& units() const { return _cus; }

private:
	void readCU(Cursor& c);
	void readHeader(Cursor& c, StateMachine& sm, CompilationUnit *cu);
	void runSM(Cursor& c, StateMachine& sm, CompilationUnit *cu, size_t end);
	void advancePC(StateMachine& sm, CompilationUnit *cu, t::uint64 adv);
	void advanceLine(StateMachine& sm, t::int64 adv);
	void recordLine(StateMachine& sm, CompilationUnit *cu);
	bool readFile(Cursor& c, StateMachine& sm, CompilationUnit *cu);
	size_t readHeaderLength(Cursor& c);
	size_t readUnitLength(Cursor& c);
	t::int64 readLEB128S(Cursor& c);
	t::uint64 readLEB128U(Cursor& c);
	inline static void error_if(bool cond)
		{ if(cond) throw gel::Exception("debug line error"); }
	address_t readAddress(Cursor& c);

	elf::File& prog;
	int addr_size, length_size;
	FragTable<CompilationUnit *> _cus;
	HashMap<sys::Path, File *> _files;
};

} }	// gel::elf

#endif	// GELPP_ELF_DEBUG_LINE_H


