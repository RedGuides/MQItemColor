# MQItemColor

Replicates MQ2SlotColors for MQNext

## Getting Started



```txt
/plugin MQItemColor
```

### Commands

No Commands Yet

```txt
Nope
```

### Configuration File

```ini
Toggles for each type. 1 for on, 0 for off.
[General]
- QuestOn=1
- TradeSkillsOn=1
- CollectibleOn=1
- NoTradeOn=1
- AttuneableOn=1

Hex codes for Item Colors based on type.
Normal is the color shown without mouseover.
Rollover is the color shown with mouseover.
[ItemColors]
- QuestNormal=0xFFF01DFF
- QuestRollover=0xFFFF35FF
- TradeSkillsNormal=0xFFF0F000
- TradeSkillsRollover=0xFFF09253
- CollectibleNormal=0xFFFF8C20
- CollectibleRollover=0xFFFFB120
- NoTradeNormal=0xFFFF2020
- NoTradeRollover=0xFFFF4120
- AttuneableNormal=0xFF6BBAFF
- AttuneableRollover=0xFFFFADF4
```

## Other Notes

Currently only supports coloring Quest, Tradeskill, Collectible, No Trade, or Attuneable items.  Coloring is top down priority.

## Authors

* **Ortster** - *Initial work*

## Acknowledgments

Thanks to the original MQ2SlotColors
