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

#ifndef _H_VANADIS_GPR_2_FP
#define _H_VANADIS_GPR_2_FP

#include "inst/vinst.h"
#include "inst/vregfmt.h"

namespace SST {
namespace Vanadis {

template<VanadisRegisterFormat register_format>
class VanadisGPR2FPInstruction : public VanadisInstruction {
public:
    VanadisGPR2FPInstruction(const uint64_t addr, const uint32_t hw_thr, const VanadisDecoderOptions* isa_opts,
                             const uint16_t fp_dest, const uint16_t int_src)
        : VanadisInstruction(addr, hw_thr, isa_opts, 1, 0, 1, 0, 0,
                             ((register_format == VanadisRegisterFormat::VANADIS_FORMAT_FP64 || register_format == VanadisRegisterFormat::VANADIS_FORMAT_INT64)
                              && (VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode()))
                                 ? 2
                                 : 1,
                             0,
                             ((register_format == VanadisRegisterFormat::VANADIS_FORMAT_FP64 || register_format == VanadisRegisterFormat::VANADIS_FORMAT_INT64)
                              && (VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode()))
                                 ? 2
                                 : 1) {
        isa_int_regs_in[0] = int_src;

        if (((register_format == VanadisRegisterFormat::VANADIS_FORMAT_FP64 || register_format == VanadisRegisterFormat::VANADIS_FORMAT_INT64)
             && (VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode()))) {

            isa_fp_regs_out[0] = fp_dest;
            isa_fp_regs_out[1] = fp_dest + 1;
        } else {
            isa_fp_regs_out[0] = fp_dest;
        }
    }

    VanadisGPR2FPInstruction* clone() override { return new VanadisGPR2FPInstruction(*this); }
    VanadisFunctionalUnitType getInstFuncType() const override { return INST_INT_ARITH; }

    const char* getInstCode() const override {
        switch (register_format) {
        case VanadisRegisterFormat::VANADIS_FORMAT_INT32:
        case VanadisRegisterFormat::VANADIS_FORMAT_FP32:
            return "GPR2FP32";
        case VanadisRegisterFormat::VANADIS_FORMAT_INT64:
        case VanadisRegisterFormat::VANADIS_FORMAT_FP64:
            return "GPR2FP64";
        }

        return "GPRCONVUNK";
    }

    void printToBuffer(char* buffer, size_t buffer_size) override {
        snprintf(buffer, buffer_size,
                 "%s fp-dest isa: %" PRIu16 " phys: %" PRIu16 " <- int-src: isa: %" PRIu16 " phys: %" PRIu16 "\n",
                 getInstCode(), isa_fp_regs_out[0], phys_fp_regs_out[0], isa_int_regs_in[0], phys_int_regs_in[0]);
    }

    void execute(SST::Output* output, VanadisRegisterFile* regFile) override {
#ifdef VANADIS_BUILD_DEBUG
        output->verbose(CALL_INFO, 16, 0,
                        "Execute (addr=0x%llx) %s fp-dest isa: %" PRIu16 " phys: %" PRIu16 " <- int-src: isa: %" PRIu16
                        " phys: %" PRIu16 "\n",
                        getInstructionAddress(), getInstCode(), isa_fp_regs_out[0], phys_fp_regs_out[0],
                        isa_int_regs_in[0], phys_int_regs_in[0]);
#endif
        switch (register_format) {
        case VanadisRegisterFormat::VANADIS_FORMAT_INT32:
        case VanadisRegisterFormat::VANADIS_FORMAT_FP32: {
            const int32_t v = regFile->getIntReg<int32_t>(phys_int_regs_in[0]);
            regFile->setFPReg<int32_t>(phys_fp_regs_out[0], v);
        } break;
        case VanadisRegisterFormat::VANADIS_FORMAT_INT64:
        case VanadisRegisterFormat::VANADIS_FORMAT_FP64: {
            if (VANADIS_REGISTER_MODE_FP32 == isa_options->getFPRegisterMode()) {
                const int64_t v = regFile->getIntReg<int64_t>(phys_int_regs_in[0]);
                fractureToRegisters<int64_t>(regFile, phys_fp_regs_out[0], phys_fp_regs_out[1], v);
            } else {
                const int64_t v = regFile->getIntReg<int64_t>(phys_int_regs_in[0]);
                regFile->setFPReg<int64_t>(phys_fp_regs_out[0], v);
            }
        } break;
        }

        markExecuted();
    }
};

} // namespace Vanadis
} // namespace SST

#endif
