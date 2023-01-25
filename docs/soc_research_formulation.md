# SoC Research Formulation

SoC design is a complex and time-consuming task because there are many areas of concern that a designer must account for when decisions are made.
In most SoC design methodologies in use today, each decision that results in a change must be cross-checked with all other areas of concern to make sure things did not break.
These cycles of Change -> Cross-Check are concerning because:
- they are usually performed manually
- they are prone to propagation errors

In this research, we want to:
- determine the nature of things/areas that are involved in the Change <--> Check cycles
- push the Change-Check cycles to the top of the SoC layering scheme
- simplify the effort needed to perform Change-Check cycles (change-check cycles are important)
- allow the soc compiler to take care of the propagation tasks resulting from Change-Check cycles

Essentially, the goal in this research to is facilitate and lift the nature of Change-Cycle tasks involved in SoC design while letting the SoC compiler to perform most of the propagation tasks. This allows:
- faster change-check cycles
- designer to more easily perform as many change-check tasks as needed

## Challenges in Modern SoC Design

- Specification
    - System Level
    - Component/IP Level
- Design Space Exploration
- Hardware Construction
- Software Design
- Design Verification
- Front-End Physical Design
- Back-End Physical Design

## Proposed SoC Design Methodology

- separation of concerns into layers
- horizontal layers
    - spec layers
    - abstract soc architecture layers
        - topology
        - IP refinement
        - etc
    - clock and reset layer
    - power control layer
    - debug layer
    - interconnect layer
    - interface legalization layer
    - design for test layer
    - interrupt layer
- vertical views/domains
    - hardware construction
    - software design
    - design verification
    - simulation / emulation
    - documentation
    - metrics: performance, estimates (power, area, etc.)

## Register Sequence Language

Objective:
----------

Provide a specification language for describing register sequences that allows:
    - portability
    - ease of processing and manipulation
    - capturing core semantics found in specs written in word documents (WHAT ARE THESE SEMANTICS?)
    - single-source of truth for
        - software design
        - hardware design
        - design verification
        - DSE?
