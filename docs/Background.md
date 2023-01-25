# SocGen: Background

In the last recent years, there has been some remarkable progress in the area of open-source SoC design tooling. With the advent of technologies such as **hardware construction languages**, libraries have been developed to assist with the design of SoCs. A notable example perhaps is the RocketChip. RocketChip is a leading example of a modern open-source SoC generator built on top of the Chisel hardware construction language.

One of the main concerns in SoC design is correctly connecting SoC IPs together. In most IPs, this usually requires the following:
- specifying correct and valid user-customizable parameters of the IP
- identifying non-changeable IP parameters
- validating IP customization based on supplied parameters and expected functionality/features/etc.
- connecting IPs together while paying attention to configuration modes and compatible features

This process of parameterizing, validating, and connecting IPs is usually done manually and is quite prone to errors. Some errors are `static` in the sense that they get revealed during IP stitching, whereas some other errors are `logical` in the sense that they make the system behave differently from the intended specification.

Select examples of `static` errors:
- signal width mismatch
- signal directionality mismatch (e.g. input -> output)
- signal not connected
- etc.

Select examples of `logical` errors:
- inverted reset polarity
- interface protocol mismatch
- insertion/removal of internal FIFOs, memories, etc
- peripheral address mismatch
- peripheral capability mismatch
- etc.

With current technologies, most `static` errors are captured using HDL syntax checkers and linters. While most `syntax` errors can be caught by these technologies, it remains a challenge for these tools to automatically fix them.
As for `logical` errors, current widespread technologies are not equipped to either capture or solve them.

RocketChip provides libraries and tools to alleviate many of these issues related to connecting IPs together. **Diplomacy** is a library that enables two modules or IPs to negotiate parameters when connected together. There 4 entities involved in a diplomatic connection:
- **diplomatic nodes**: these are the entities that provide IP parameters for each pair-wise connection they participate in. A module/IP can have as many diplomatic nodes as necessary
- **brokers** : there are the entities that generate final agreed-upon parameter bindings based on parameters received from each side of a connection. In `diplomacy` terms, these are `NodeImp` objects
- **Final Parameters**: these are the final parameters obtained from a `broker`. In `diplomacy` terms, these are called `edge parameters`
- **Connection Interfaces**: These are the interfaces that are used to physically connect the two modules/IPs. They are generated based on the agreed-upon `final/edge parameters`. In `diplomacy` terms, these are the hardware `Chisel` bundles used to connect the two modules

With `diplomacy`, information is passed along the two connecting IPs and therefore, both `static` and `logical` errors can be avoided by having both connecting IPs customize their interfaces and internal logic based on the negotiated information/parameters. With `diplomacy` being an independent library from RocketChip, it can be used in any hardware construction project based on Chisel.

RocketChip is an SoC generator targeting RISC-V cores and TileLink interconnect. Most of its generated collateral is framed within that assumption. Therefore, it is valid to say that RocketChip is a generator that alleviate the task of creating SoCs based on RISC-V cores and TileLink interconnects.

**SocGen** aims to provide utilities, tools, and libraries that are SoC-architecture independent and can be used to build any type of SoC. However, in this first version of the project, we will be targeting embedded SoCs first.
The objectives of SoCGen are:
- facilitate IP interconnection (as enabled by `diplomacy`)
- provide an IP specification library : this helps provide pertinent information about an IP that can facilitate many of the issues found at the system level
- facilitate software collateral generation
- facilitate validation/verification collateral generation
- facilitate physical design collateral generation
- facilitate system documentation generation
- enable the use of IPs in different views (RTL, simulation, emulation, etc)
