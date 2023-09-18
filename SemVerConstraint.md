# Semantic Versioning Constraint Spec

Very similar to https://getcomposer.org/doc/articles/versions.md

## Grammar

The following grammar is specified in Extended Backus-Naur Form

```scss
FILTER = constraint;

positive_digit = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
digit = "0" | positive_digit;
digits = { digit };
whitespace = " ", { " " };

release_type = "-alpha" | "-beta" | "-gamma" | "-rc";

number = "0" | positive_digit, { digit };
version_number = number, ".", ( number, [ ".", ( number | "*" ) ] | "*" ), [ release_type ];

constraint_type = "=" | "!=" | "<" | "<=" | ">" | ">=" | "~" | "^";
constraint_or = "||";

single_version_constraint = [ constraint_type ], [ whitespace ], version_number;
and_constraint = single_version_constraint, { [ whitespace ], single_version_constraint };
or_constraint = and_constraint, { [ whitespace ], constraint_or, [ whitespace ], and_constraint };

constraint = or_constraint;
```

