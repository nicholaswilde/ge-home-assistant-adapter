#!/usr/bin/env python3
import json
import os
import urllib.request
import urllib.error
import sys

def download_file(url, dest_path):
    print(f"Checking {url} ...")
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        with urllib.request.urlopen(req, timeout=5) as response:
            content = response.read()
            with open(dest_path, 'wb') as f:
                f.write(content)
            print(f"Successfully downloaded to {dest_path}")
    except Exception as e:
        if os.path.exists(dest_path):
            print(f"Failed to download, using cached version. Error: {e}")
        else:
            print(f"Failed to download and no cached version exists! Error: {e}")
            sys.exit(1)

def main():
    api_url = "https://raw.githubusercontent.com/geappliances/public-appliance-api-documentation/main/appliance_api.json"
    erd_url = "https://raw.githubusercontent.com/geappliances/public-appliance-api-documentation/main/appliance_api_erd_definitions.json"
    
    cache_dir = os.path.join(os.path.dirname(__file__), "..", ".cache")
    os.makedirs(cache_dir, exist_ok=True)
    
    api_json_path = os.path.join(cache_dir, "appliance_api.json")
    erd_json_path = os.path.join(cache_dir, "appliance_api_erd_definitions.json")
    
    download_file(api_url, api_json_path)
    download_file(erd_url, erd_json_path)
    
    src_generated_dir = os.path.join(os.path.dirname(__file__), "..", "src", "generated")
    os.makedirs(src_generated_dir, exist_ok=True)
    
    with open(api_json_path, 'r') as f:
        api_data = json.load(f)
        
    appliance_types = []
    for key, value in api_data.get("featureApis", {}).items():
        appliance_types.append((int(key), value.get("name", "Unknown").replace('"', '\\"')))
    appliance_types.sort(key=lambda x: x[0])
    
    with open(erd_json_path, 'r') as f:
        erd_data = json.load(f)
    erds = erd_data.get("erds", [])
    
    h_file_path = os.path.join(src_generated_dir, "ErdParser.h")
    cpp_file_path = os.path.join(src_generated_dir, "ErdParser.cpp")
    
    with open(h_file_path, 'w') as f:
        f.write("#pragma once\n#include <Arduino.h>\n#include <PubSubClient.h>\n\n")
        f.write("struct ApplianceType {\n  uint16_t id;\n  const char* name;\n};\n\n")
        f.write("extern const ApplianceType applianceTypes[];\n")
        f.write("extern const size_t applianceTypesCount;\n\n")
        f.write("String decodeErdToJson(uint16_t erd, const uint8_t* data, uint8_t size);\n")
        f.write("void publishHomeAssistantDiscovery(PubSubClient& mqtt, uint16_t erd_id, const String& deviceId, const String& applianceId);\n")
        
    with open(cpp_file_path, 'w') as f:
        f.write("#include \"ErdParser.h\"\n#include <ArduinoJson.h>\n\n")
        f.write("const ApplianceType applianceTypes[] = {\n")
        for type_id, type_name in appliance_types:
            f.write(f'  {{{type_id}, "{type_name}"}},\n')
        f.write("};\n")
        f.write("const size_t applianceTypesCount = sizeof(applianceTypes) / sizeof(applianceTypes[0]);\n\n")
        
        f.write("String decodeErdToJson(uint16_t erd, const uint8_t* data, uint8_t size) {\n")
        f.write("  StaticJsonDocument<512> doc;\n")
        f.write("  switch (erd) {\n")
        
        for erd in erds:
            erd_id_str = erd.get("id", "0x0000")
            erd_id = int(erd_id_str, 16)
            data_fields = erd.get("data", [])
            if not data_fields:
                continue
                
            f.write(f"    case {erd_id_str}: {{\n")
            for field in data_fields:
                name = field.get("name", "Unknown").replace('"', '\\"')
                t = field.get("type", "raw")
                offset = field.get("offset", 0)
                size_field = field.get("size", 1)
                
                if t == "string":
                    f.write(f'      if (size >= {offset + size_field}) {{\n        char buf[{size_field}+1] = {{0}};\n        memcpy(buf, &data[{offset}], {size_field});\n        doc["{name}"] = buf;\n      }}\n')
                elif t in ["u8", "i8"] and size_field == 1:
                    f.write(f'      if (size >= {offset + 1}) doc["{name}"] = data[{offset}];\n')
                elif t in ["u16", "i16"] and size_field == 2:
                    f.write(f'      if (size >= {offset + 2}) doc["{name}"] = (data[{offset}] << 8) | data[{offset+1}];\n')
                elif t in ["bool"] and size_field == 1:
                    f.write(f'      if (size >= {offset + 1}) doc["{name}"] = data[{offset}] ? true : false;\n')
            f.write("      break;\n    }\n")
            
        f.write("    default: {\n")
        f.write("      String rawHex;\n")
        f.write("      for(int i=0; i<size; i++) {\n")
        f.write("        char buf[3];\n")
        f.write("        snprintf(buf, sizeof(buf), \"%02x\", data[i]);\n")
        f.write("        rawHex += buf;\n")
        f.write("      }\n")
        f.write("      doc[\"raw\"] = rawHex;\n")
        f.write("      break;\n")
        f.write("    }\n  }\n")
        f.write("  String rawHex;\n")
        f.write("  for(int i=0; i<size; i++) {\n")
        f.write("    char buf[3];\n")
        f.write("    snprintf(buf, sizeof(buf), \"%02x\", data[i]);\n")
        f.write("    rawHex += buf;\n")
        f.write("  }\n")
        f.write("  doc[\"raw\"] = rawHex;\n")
        f.write("  String output;\n  serializeJson(doc, output);\n  return output;\n}\n\n")
        
        f.write("void publishHomeAssistantDiscovery(PubSubClient& mqtt, uint16_t erd_id, const String& deviceId, const String& applianceId) {\n")
        f.write("  char topic[128];\n")
        f.write("  char erd_hex[16];\n")
        f.write("  snprintf(erd_hex, sizeof(erd_hex), \"%04x\", erd_id);\n")
        f.write("  snprintf(topic, sizeof(topic), \"homeassistant/sensor/%s/erd_%04x/config\", deviceId.c_str(), erd_id);\n")
        f.write("  StaticJsonDocument<512> doc;\n")
        f.write("  doc[\"name\"] = String(\"ERD \") + String(erd_hex);\n")
        f.write("  doc[\"state_topic\"] = \"geappliances/\" + deviceId + \"/erd/0x\" + String(erd_hex) + \"/state\";\n")
        f.write("  doc[\"value_template\"] = \"{{ value_json.raw | default('OK') }}\";\n")
        f.write("  doc[\"json_attributes_topic\"] = \"geappliances/\" + deviceId + \"/erd/0x\" + String(erd_hex) + \"/state\";\n")
        f.write("  doc[\"unique_id\"] = deviceId + \"_erd_\" + String(erd_hex);\n")
        f.write("  JsonObject dev = doc.createNestedObject(\"device\");\n")
        f.write("  dev[\"identifiers\"].add(deviceId);\n")
        f.write("  dev[\"name\"] = \"GE Appliance \" + deviceId;\n")
        f.write("  dev[\"model\"] = applianceId;\n")
        f.write("  String payload;\n")
        f.write("  serializeJson(doc, payload);\n")
        f.write("  mqtt.publish(topic, payload.c_str(), true);\n")
        f.write("}\n")

if __name__ == "__main__":
    main()
