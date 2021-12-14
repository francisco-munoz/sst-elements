![SST](http://sst-simulator.org/img/sst-logo-small.png)
![SST](https://github.com/stonne-simulator/sst-elements-with-stonne/blob/master/stonne-small.jpg)

# Structural Simulation Toolkit (SST)

#### Copyright (c) 2009-2018, National Technology and Engineering Solutions of Sandia, LLC (NTESS)
Portions are copyright of other developers:
See the file CONTRIBUTORS.TXT in the top level directory
of this repository for more information.

---

The Structural Simulation Toolkit (SST) was developed to explore innovations in highly concurrent systems where the ISA, microarchitecture, and memory interact with the programming model and communications system. The package provides two novel capabilities. The first is a fully modular design that enables extensive exploration of an individual system parameter without the need for intrusive changes to the simulator. The second is a parallel simulation environment based on MPI. This provides a high level of performance and the ability to look at large systems. The framework has been successfully used to model concepts ranging from processing in memory to conventional processors connected by conventional network interfaces and running MPI.

Visit [sst-simulator.org](http://sst-simulator.org) to learn more about SST.

See [Contributing](https://github.com/sstsimulator/sst-elements/blob/devel/CONTRIBUTING.md) to learn how to contribute to SST.

##### [LICENSE](https://github.com/sstsimulator/sst-elements/blob/devel/LICENSE)


# STONNE Simulator

---

STONNE is a cycle-level microarchitectural simulator for flexible DNN inference accelerators. STONNE models all the major components required to build both first-generation rigid accelerators and next-generation flexible DNN accelerators. All the on-chip components are interconnected by using a three-tier network fabric composed of a Distribution Network(DN), a Multiplier Network (MN), and a Reduce Network(RN), inspired by the taxonomy of on-chip communication flows within DNN accelerators. These networks canbe configured to support any topology. Next, we describe the different topologies of the three networks (DN, MN and RN) currently supported in STONNE that are basic building blocks of state-of-the-art accelerators such as the Google’s TPU, Eyeriss-v2, ShDianNao, SCNN, MAERI and SIGMA, among others. These building blocks can also be seen in the figure presented below:

![alt text](https://github.com/francisco-munoz/stonne/blob/master/figures/STONNE_components.png)


Visit [stonne](https://github.com/stonne-simulator/stonne) to learn more about STONNE Simulator.

---

# SST-STONNE simulator

STONNE has been integrated as a component called sstStonne in SST. sstStonne connects to memHierarchy component to faithfully model the memory hierarchy of the accelerator. A schematic view of sstStonne is presented below:

![alt text](https://github.com/stonne-simulator/sst-elements-with-stonne/blob/master/sstStonne-memHierarchy.jpg)

## How to install sstStonne

Please install sst-elements-with-stonne following the installation description presented in [sst-simulator.org](http://sst-simulator.org). 

## How to run sstStonne

sstStonne must be instanstiated in the SST Configuration file, along with the memory hierarchy components and subcomponents that will be used to model the memory hierarchy. Please refer to our examples for further information:

- sst configuration file to run a convolution operation: tests/sst_stonne_conv.py
- sst configuration file to run a dense GEMM operation: tests/sst_stonne_gemm.py
- sst configuration file to run a sparse-sparse GEMM operation where the matrices are encoded using a bitmat format: tests/sst_stonne_bitmapSpMSpM.py
- sst configuration file to run a sparse-dense GEMM operation where the sparse matrix is encoded using a CSR format: tests/sst_stonne_csrSpMM.py

Also, refer to our scripts to generate the memory initialization files and calculate the memory address locations: 
- Script for convolution operation: tests/gen_conv.py
- Script for dense GEMM operation: tests/gen_gemm.py
- Script for sparse-sparse GEMM operation where the matrices are encoded using a bitmat format: tests/gen_bitmapSpMSpM.py

Please remember to update the SST configuration file with the proper memory address locations and the proper memory intialization file for each kernel to be launched. 
