#ifndef PLUGIN_SYSTEM_H
#define PLUGIN_SYSTEM_H

#include "astro_types.h"
#include "birth_chart.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace Astro {

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;

    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual std::string getCategory() const = 0; // Interpretation, Calculation, Visualization, etc.

    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isCompatible(const std::string& coreVersion) const = 0;

    // Plugin capabilities
    virtual std::vector<std::string> getCommands() const = 0;
    virtual std::vector<std::string> getOutputFormats() const = 0;
    virtual std::vector<std::string> getFeatures() const = 0;

    // Plugin execution
    virtual std::string execute(const std::string& command, const std::map<std::string, std::string>& parameters) = 0;
    virtual bool processChart(const BirthChart& chart, std::map<std::string, std::string>& results) = 0;

    // Configuration
    virtual void setConfiguration(const std::map<std::string, std::string>& config) = 0;
    virtual std::map<std::string, std::string> getConfiguration() const = 0;
    virtual std::vector<std::string> getConfigurationKeys() const = 0;
};

// Plugin types
enum class PluginType {
    INTERPRETATION,    // AI interpretation, traditional interpretation
    CALCULATION,       // Custom calculations, new chart types
    VISUALIZATION,     // Chart drawing, graphs, animations
    PREDICTION,        // Predictive algorithms, timing systems
    CALENDAR,          // Calendar systems, ephemeris
    COMPATIBILITY,     // Synastry, composite, relationship analysis
    REMEDIAL,          // Remedial measures, suggestions
    EXPORT,            // Output formats, file generation
    INTEGRATION,       // API integrations, external services
    UTILITY            // Helper tools, converters
};

struct PluginInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string website;
    std::string license;
    PluginType type;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
    bool isLoaded;
    bool isEnabled;
    std::string loadPath;
    std::string configPath;
};

class PluginManager {
private:
    std::map<std::string, std::shared_ptr<IPlugin>> loadedPlugins;
    std::map<std::string, PluginInfo> pluginRegistry;
    std::vector<std::string> pluginPaths;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;

    // Plugin loading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginId);
    bool validatePlugin(const std::shared_ptr<IPlugin>& plugin);
    bool checkDependencies(const std::vector<std::string>& dependencies);

    // Dynamic library loading
    void* loadLibrary(const std::string& libraryPath);
    void unloadLibrary(void* handle);
    void* getSymbol(void* handle, const std::string& symbolName);

public:
    PluginManager();
    ~PluginManager();

    // Plugin management
    bool loadPluginFromFile(const std::string& filePath);
    bool loadPluginFromDirectory(const std::string& directoryPath);
    void loadAllPlugins();
    void unloadAllPlugins();

    // Plugin registry
    std::vector<PluginInfo> getAvailablePlugins() const;
    std::vector<PluginInfo> getLoadedPlugins() const;
    std::vector<PluginInfo> getEnabledPlugins() const;
    std::vector<PluginInfo> getPluginsByType(PluginType type) const;
    PluginInfo getPluginInfo(const std::string& pluginId) const;

    // Plugin operations
    bool enablePlugin(const std::string& pluginId);
    bool disablePlugin(const std::string& pluginId);
    bool reloadPlugin(const std::string& pluginId);
    bool hasPlugin(const std::string& pluginId) const;

    // Plugin execution
    std::string executePluginCommand(const std::string& pluginId, const std::string& command,
                                   const std::map<std::string, std::string>& parameters);
    bool processChartWithPlugin(const std::string& pluginId, const BirthChart& chart,
                               std::map<std::string, std::string>& results);

    // Plugin discovery
    void addPluginPath(const std::string& path);
    void removePluginPath(const std::string& path);
    std::vector<std::string> getPluginPaths() const;
    void scanForPlugins();

    // Configuration
    void setPluginConfiguration(const std::string& pluginId, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfiguration(const std::string& pluginId) const;
    void savePluginConfigurations();
    void loadPluginConfigurations();

    // Plugin marketplace
    std::vector<PluginInfo> getAvailableOnlinePlugins();
    bool downloadPlugin(const std::string& pluginId, const std::string& downloadUrl);
    bool installPlugin(const std::string& packagePath);
    bool uninstallPlugin(const std::string& pluginId);
    bool updatePlugin(const std::string& pluginId);

    // Plugin validation and security
    bool validatePluginSignature(const std::string& pluginPath);
    bool isPluginTrusted(const std::string& pluginId);
    void setPluginTrust(const std::string& pluginId, bool trusted);
    std::vector<std::string> getSecurityWarnings(const std::string& pluginId);

    // Error handling
    std::vector<std::string> getPluginErrors() const;
    std::string getLastError() const;
    void clearErrors();
};

// Built-in plugin implementations
class InterpretationPlugin : public IPlugin {
private:
    std::map<std::string, std::string> config;

public:
    std::string getName() const override { return "Advanced Interpretation Engine"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "AI-powered astrological interpretation"; }
    std::string getAuthor() const override { return "Horoscope CLI Team"; }
    std::vector<std::string> getDependencies() const override { return {}; }
    std::string getCategory() const override { return "interpretation"; }

    bool initialize() override;
    void shutdown() override;
    bool isCompatible(const std::string& coreVersion) const override;

    std::vector<std::string> getCommands() const override;
    std::vector<std::string> getOutputFormats() const override;
    std::vector<std::string> getFeatures() const override;

    std::string execute(const std::string& command, const std::map<std::string, std::string>& parameters) override;
    bool processChart(const BirthChart& chart, std::map<std::string, std::string>& results) override;

    void setConfiguration(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfiguration() const override;
    std::vector<std::string> getConfigurationKeys() const override;
};

class VisualizationPlugin : public IPlugin {
private:
    std::map<std::string, std::string> config;

public:
    std::string getName() const override { return "Advanced Chart Visualization"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Enhanced chart drawing and visualization"; }
    std::string getAuthor() const override { return "Horoscope CLI Team"; }
    std::vector<std::string> getDependencies() const override { return {}; }
    std::string getCategory() const override { return "visualization"; }

    bool initialize() override;
    void shutdown() override;
    bool isCompatible(const std::string& coreVersion) const override;

    std::vector<std::string> getCommands() const override;
    std::vector<std::string> getOutputFormats() const override;
    std::vector<std::string> getFeatures() const override;

    std::string execute(const std::string& command, const std::map<std::string, std::string>& parameters) override;
    bool processChart(const BirthChart& chart, std::map<std::string, std::string>& results) override;

    void setConfiguration(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfiguration() const override;
    std::vector<std::string> getConfigurationKeys() const override;
};

class ExportPlugin : public IPlugin {
private:
    std::map<std::string, std::string> config;

public:
    std::string getName() const override { return "Advanced Export Engine"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Multiple export formats and integrations"; }
    std::string getAuthor() const override { return "Horoscope CLI Team"; }
    std::vector<std::string> getDependencies() const override { return {}; }
    std::string getCategory() const override { return "export"; }

    bool initialize() override;
    void shutdown() override;
    bool isCompatible(const std::string& coreVersion) const override;

    std::vector<std::string> getCommands() const override;
    std::vector<std::string> getOutputFormats() const override;
    std::vector<std::string> getFeatures() const override;

    std::string execute(const std::string& command, const std::map<std::string, std::string>& parameters) override;
    bool processChart(const BirthChart& chart, std::map<std::string, std::string>& results) override;

    void setConfiguration(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfiguration() const override;
    std::vector<std::string> getConfigurationKeys() const override;
};

// Plugin helper functions
class PluginHelpers {
public:
    // Command line integration
    static std::vector<std::string> getPluginCommands();
    static bool executePluginFromCommandLine(int argc, char* argv[]);

    // Plugin development
    static bool createPluginTemplate(const std::string& pluginName, const std::string& outputPath);
    static bool validatePluginStructure(const std::string& pluginPath);
    static std::string generatePluginManifest(const PluginInfo& info);

    // Plugin packaging
    static bool packagePlugin(const std::string& pluginPath, const std::string& outputPath);
    static bool extractPlugin(const std::string& packagePath, const std::string& extractPath);

    // Plugin marketplace
    static std::vector<PluginInfo> searchOnlinePlugins(const std::string& query);
    static bool submitPlugin(const std::string& pluginPath, const std::string& marketplaceUrl);
    static std::vector<std::string> getPluginReviews(const std::string& pluginId);

    // Plugin testing
    static bool runPluginTests(const std::string& pluginId);
    static std::vector<std::string> getPluginTestResults(const std::string& pluginId);
    static bool benchmarkPlugin(const std::string& pluginId);
};

// Plugin events system
enum class PluginEvent {
    PLUGIN_LOADED,
    PLUGIN_UNLOADED,
    PLUGIN_ENABLED,
    PLUGIN_DISABLED,
    PLUGIN_ERROR,
    CHART_PROCESSED,
    COMMAND_EXECUTED
};

using PluginEventHandler = std::function<void(PluginEvent event, const std::string& pluginId, const std::map<std::string, std::string>& data)>;

class PluginEventManager {
private:
    std::map<PluginEvent, std::vector<PluginEventHandler>> eventHandlers;

public:
    void registerEventHandler(PluginEvent event, PluginEventHandler handler);
    void unregisterEventHandler(PluginEvent event, PluginEventHandler handler);
    void fireEvent(PluginEvent event, const std::string& pluginId, const std::map<std::string, std::string>& data = {});
};

} // namespace Astro

#endif // PLUGIN_SYSTEM_H
