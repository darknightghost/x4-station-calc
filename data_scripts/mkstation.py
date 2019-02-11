#! /usr/bin/env python3

template = {
    "id": "",
    "type": "",
    "factions": ["FACTION_ARGON", "FACTION_TELADI", "FACTION_PARANID"],
    "turret": 0,
    "name": {
        "en_US": "",
        "zh_CN": ""
    }
}

import pathlib
import sys
import json

basedir = pathlib.Path(sys.argv[1])
enLines = []
zhLines = []

with open(str(basedir / "en.txt")) as f:
    enLines = f.readlines()

with open(str(basedir / "zh.txt")) as f:
    zhLines = f.readlines()

for i in range(0, len(enLines)):
    en = enLines[i].strip()
    zh = zhLines[i].strip()
    stationId = en.replace(" ", "_").upper()
    stationType = "Other"
    template["id"] = stationId
    template["type"] = stationType
    template["name"]["en_US"] = en
    template["name"]["zh_CN"] = zh
    s = json.dumps(template, ensure_ascii=False)
    fname = basedir / (stationId.lower() + ".json")
    with open(str(fname), "w") as f:
        f.write(s)
