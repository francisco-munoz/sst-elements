![SST](http://sst-simulator.org/img/sst-logo-small.png)
![SST](https://github.com/stonne-simulator/sst-elements-with-stonne/blob/master/stonne-small.jpg)

# Structural Simulation Toolkit (SST)

#### Copyright (c) 2009-2018, National Technology and Engineering Solutions of Sandia, LLC (NTESS)
Portions are copyright of other developers:
See the file CONTRIBUTORS.TXT in the top level directory
of this repository for more information.

---

The Structural Simulation Toolkit (SST) was developed to explore innovations in highly concurrent systems where the ISA, microarchitecture, and memory interact with the programming model and communications system. The package provides two novel capabilities. The first is a fully modular design that enables extensive exploration of an individual system parameter without the need for intrusive changes to the simulator. The second is a parallel simulation environment based on MPI. This provides a high level of performance and the ability to look at large systems. The framework has been successfully used to model concepts ranging from processing in memory to conventional processors connected by conventional network interfaces and running MPI.

---

---

STONNE is a cycle-level microarchitectural simulator for flexible DNN inference accelerators. STONNE models all the major components required to build both first-generation rigid accelerators and next-generation flexible DNN accelerators. All the on-chip components are interconnected by using a three-tier network fabric composed of a Distribution Network(DN), a Multiplier Network (MN), and a Reduce Network(RN), inspired by the taxonomy of on-chip communication flows within DNN accelerators. These networks canbe configured to support any topology. Next, we describe the different topologies of the three networks (DN, MN and RN) currently supported in STONNE that are basic building blocks of state-of-the-art accelerators such as the Google’s TPU, Eyeriss-v2, ShDianNao, SCNN, MAERI and SIGMA, among others. These building blocks can also be seen in the figure presented below:

![alt text](https://github.com/francisco-munoz/stonne/blob/master/figures/STONNE_components.png)

---

Visit [sst-simulator.org](http://sst-simulator.org) to learn more about SST.

See [Contributing](https://github.com/sstsimulator/sst-elements/blob/devel/CONTRIBUTING.md) to learn how to contribute to SST.

##### [LICENSE](https://github.com/sstsimulator/sst-elements/blob/devel/LICENSE)
