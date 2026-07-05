#import "@local/ieee-paper:1.0.0": ieee

#show: ieee.with(
  title: [],
  abstract: [],
  index-terms: ("",),
  authors: (
    (
      name: "",
      department: [],
      organization: [],
      location: [],
      email: "",
    ),
  ),
  bibliography: bibliography("refs.bib"),
  lang: "en",
)

= Introduction
