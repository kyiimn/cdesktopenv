# Decisions - Style Manager Font Family

## 2026-06-15
- fontChoice[10] → fontChoice[MAX_FONT_FAMILIES * MAX_FONT_SIZES] (flat 1D, 56 slots)
- MAX_FONT_FAMILIES = 8, MAX_FONT_SIZES = 7
- FONT_INDEX(fam, sz) = fam * MAX_FONT_SIZES + sz
- Family 0 = alias to existing SystemFont1..7/UserFont1..7
- *FontFamily: N resource added to fontres (additive only, G3/G4)
- Session saves *Fonts.familyIndex: N for restore
- numFamilies default = 2 (system, user)
- Horizontal layout: familyTB (left) + sizeTB (right)