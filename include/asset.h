#pragma once

#include <SDL2/SDL.h>

#include <unordered_map>
#include <string>
#include <iostream>

#include "pugixml.hpp"

class AssetSystem;

class AssetDescriptor {
public:
  AssetDescriptor() {}
  virtual ~AssetDescriptor() {}
  virtual void *load(const AssetSystem& sys, const pugi::xml_node& node) = 0;
};

typedef std::unordered_map<std::string, std::weak_ptr<void>> AssetMap;
typedef std::unordered_map<std::string, AssetDescriptor*> AssetDescriptorMap;

class AssetSystem {
public:
  AssetSystem() {

  }

  ~AssetSystem() {
    for (AssetDescriptorMap::iterator it = m_descriptorMap.begin(); it != m_descriptorMap.end(); ++it) {
      delete it->second;
      m_descriptorMap.erase(it);
    }
  }

  void registerDescriptor(std::string id, AssetDescriptor* descriptor) {
    m_descriptorMap.insert(std::pair<std::string, AssetDescriptor*>(id, descriptor));
  }

  void removeDescriptor(std::string id) {
    AssetDescriptorMap::iterator it = m_descriptorMap.find(id);
    if (it != m_descriptorMap.end()) {
      delete it->second;
      m_descriptorMap.erase(it);
    }
  }

  bool addCatalogue(std::string path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());

    if (result) {
      pugi::xml_node assets = doc.root().child("assets");
      if (!assets) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load asset catalogue '%s': missing <assets> element", path.c_str());
        return false;
      }

      int count = 0;

      for (pugi::xml_node_iterator it = assets.begin(); it != assets.end(); ++it) {
        m_doc.append_copy(*it);
        ++count;
      }

      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Parsed asset catalogue '%s': %i items", path.c_str(), count);
      return true;
    } else {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load asset catalogue '%s': %s", path.c_str(), result.description());
      return false;
    }

  }

  template<typename T>
  std::shared_ptr<T> getAsset(std::string id, std::shared_ptr<T> fallback = nullptr) {
    if (m_assetMap.count(id) > 0) {
      std::shared_ptr<T> asset = std::static_pointer_cast<T>(m_assetMap.at(id).lock());
      if (asset) return asset;
      else SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Weak reference to asset '%s' has expired", id.c_str());
    }

    pugi::xml_node assetNode = m_doc.find_child_by_attribute("id", id.c_str());
    if (!assetNode) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find an asset with id '%s'", id.c_str());
      return fallback ? fallback : nullptr;
    }

    if (m_descriptorMap.count(assetNode.name()) == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find an asset descriptor for <%s>", assetNode.name());
      return fallback ? fallback : nullptr;
    }

    AssetDescriptor *descriptor = m_descriptorMap.at(assetNode.name());
    std::shared_ptr<T> asset(static_cast<T*>(descriptor->load(*this, assetNode)));
    std::weak_ptr<T> weakRef(asset);

    m_assetMap.insert(std::pair<std::string, std::weak_ptr<void>>(id, weakRef));

    return std::static_pointer_cast<T>(asset);
  }
private:
  pugi::xml_document m_doc;
  AssetMap m_assetMap;
  AssetDescriptorMap m_descriptorMap;
};
