# SocGen: Cake Pattern

Just like `RocketChip`, **SocGen** provides a suite of `Scala Traits` that help
capture IP-level specifications and system-level specifications.

One of the main issues that come up with SoC and IP specifications is that the amount
of detail information that needs to be provided can easily become too much. Like `RocketChip`,
we want to provide reasonable default values and values derived from IP interconnections using
scala traits (Cake Pattern).

We anticipate that there are going to be two classes of specifications:
- system-level
- IP-level

The root of all system-level traits is `SoCLike`, whereas the root of all IP-level traits is
`IpLike`
