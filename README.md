# MQItemColor

Replicates MQ2SlotColors for MQNext

## Getting Started


Load the plugin the normal way
```txt
/plugin MQItemColor
```

Since this plugin loads an XML, a UI reload may be required.

### Commands

No Commands Yet

### Configuration File


Toggles for each type. 1 for on, 0 for off.

```ini
[General]
QuestOn=1
TradeSkillsOn=1
CollectibleOn=1
NoTradeOn=1
AttuneableOn=1
```

Hex codes for Item Colors based on type.
Normal is the color shown without mouseover.
Rollover is the color shown with mouseover.

```ini
[ItemColors]
QuestNormal=0xFFF01DFF
QuestRollover=0xFFF9AFFF
TradeSkillsNormal=0xFFF0F000
TradeSkillsRollover=0xFFF09253
CollectibleNormal=0xFFFF8C20
CollectibleRollover=0xFFFFCA4D
NoTradeNormal=0xFFFF2020
NoTradeRollover=0xFFFF8080
AttuneableNormal=0xFF6BBAFF
AttuneableRollover=0xFFFFADF4
```

## Other Notes

Currently only supports coloring Quest, Tradeskill, Collectible, No Trade, or Attuneable items.  Coloring is top down priority.

## Authors

* **Ortster** - *Initial work*

