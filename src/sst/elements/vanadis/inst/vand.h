// Copyright 2009-2021 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2021, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef _H_VANADIS_AND
#define _H_VANADIS_AND

#include "inst/vinst.h"

namespace SST {
namespace Vanadis {

class VanadisAndInstruction : public VanadisInstruction {
public:
    VanadisAndInstruction(const uint64_t addr, const uint32_t hw_thr, const VanadisDecoderOptions* isa_opts,
                          const uint16_t dest, const uint16_t src_1, const uint16_t src_2)
        : VanadisInstruction(addr, hw_thr, isa_opts, 2, 1, 2, 1, 0, 0, 0, 0) {

        isa_int_regs_in[0] = src_1;
        isa_int_regs_in[1] = src_2;
        isa_int_regs_out[0] = dest;
    }

    VanadisAndInstruction* clone() override { return new VanadisAndInstruction(*this); }
    VanadisFunctionalUnitType getInstFuncType() const override { return INST_INT_ARITH; }
    const char* getInstCode() const override { return "AND"; }

    void printToBuffer(char* buffer, size_t buffer_size) override {
        snprintf(buffer, buffer_size,
                 "AND     %5" PRIu16 " <- %5" PRIu16 " + %5" PRIu16 " (phys: %5" PRIu16 " <- %5" PRIu16 " + %5" PRIu16
                 ")",
                 isa_int_regs_out[0], isa_int_regs_in[0], isa_int_regs_in[1], phys_int_regs_out[0], phys_int_regs_in[0],
                 phys_int_regs_in[1]);
    }

    void execute(SST::Output* output, VanadisRegisterFile* regFile) override {
#ifdef VANADIS_BUILD_DEBUG
        output->verbose(CALL_INFO, 16, 0,
                        "Execute: (addr=%p) AND phys: out=%" PRIu16 " in=%" PRIu16 ", %" PRIu16 ", isa: out=%" PRIu16
                        " / in=%" PRIu16 ", %" PRIu16 "\n",
                        (void*)getInstructionAddress(), phys_int_regs_out[0], phys_int_regs_in[0], phys_int_regs_in[1],
                        isa_int_regs_out[0], isa_int_regs_in[0], isa_int_regs_in[1]);
#endif
        const uint64_t src_1 = regFile->getIntReg<uint64_t>(phys_int_regs_in[0]);
        const uint64_t src_2 = regFile->getIntReg<uint64_t>(phys_int_regs_in[1]);

        regFile->setIntReg<uint64_t>(phys_int_regs_out[0], (src_1 & src_2));

        markExecuted();
    }
};

} // namespace Vanadis
} // namespace SST

#endif
