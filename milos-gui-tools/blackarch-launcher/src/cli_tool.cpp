#include <CLI/CLI.hpp>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("milos-blackarch-launcher");

    CLI::App cliApp{"MilOS BlackArch Launcher CLI Tool", "milos-blackarch-launcher"};
    cliApp.require_subcommand(0, 1);

    // Launch command
    auto* launchCmd = cliApp.add_subcommand("launch", "Launch tool");
    std::string launchToolName;
    launchCmd->add_option("tool-name", launchToolName, "Tool name to launch")->required();

    // List command
    auto* listCmd = cliApp.add_subcommand("list", "List tools");

    // Search command
    auto* searchCmd = cliApp.add_subcommand("search", "Search tools");
    std::string searchQuery;
    searchCmd->add_option("query", searchQuery, "Search query")->required();

    // Info command
    auto* infoCmd = cliApp.add_subcommand("info", "Get tool information");
    std::string infoToolName;
    infoCmd->add_option("tool-name", infoToolName, "Tool name")->required();

    // Categories command
    auto* categoriesCmd = cliApp.add_subcommand("categories", "List categories");

    // Category command
    auto* categoryCmd = cliApp.add_subcommand("category", "Get tools by category");
    std::string categoryName;
    categoryCmd->add_option("category", categoryName, "Category name")->required();

    // Favorites commands
    auto* favoritesCmd = cliApp.add_subcommand("favorites", "Manage favorites");
    auto* addFavoriteCmd = favoritesCmd->add_subcommand("add", "Add tool to favorites");
    std::string addFavoriteTool;
    addFavoriteCmd->add_option("tool-name", addFavoriteTool, "Tool name")->required();
    
    auto* removeFavoriteCmd = favoritesCmd->add_subcommand("remove", "Remove tool from favorites");
    std::string removeFavoriteTool;
    removeFavoriteCmd->add_option("tool-name", removeFavoriteTool, "Tool name")->required();
    
    auto* listFavoritesCmd = favoritesCmd->add_subcommand("list", "List favorite tools");

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cliApp.exit(e);
    }

    // Create D-Bus interface
    QDBusInterface interface("org.milos.BlackArchLauncher",
                            "/org/milos/BlackArchLauncher",
                            "org.milos.BlackArchLauncher",
                            QDBusConnection::sessionBus());

    if (!interface.isValid()) {
        std::cerr << "Error: Cannot connect to BlackArch Launcher service." << std::endl;
        std::cerr << "Make sure the service is running: milos-blackarch-launcher" << std::endl;
        return 1;
    }

    // Execute commands
    if (*launchCmd) {
        QDBusReply<bool> reply = interface.call("LaunchTool", QString::fromStdString(launchToolName));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        if (reply.value()) {
            std::cout << "Tool launched: " << launchToolName << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to launch tool." << std::endl;
            return 1;
        }
    } else if (*listCmd) {
        QDBusReply<QStringList> reply = interface.call("ListTools");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QStringList tools = reply.value();
        std::cout << "Tools (" << tools.size() << "):" << std::endl;
        for (const QString& tool : tools) {
            std::cout << "  - " << tool.toStdString() << std::endl;
        }
        return 0;
    } else if (*searchCmd) {
        QDBusReply<QStringList> reply = interface.call("SearchTools", QString::fromStdString(searchQuery));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QStringList tools = reply.value();
        std::cout << "Search results (" << tools.size() << "):" << std::endl;
        for (const QString& tool : tools) {
            std::cout << "  - " << tool.toStdString() << std::endl;
        }
        return 0;
    } else if (*infoCmd) {
        QDBusReply<QString> reply = interface.call("GetToolInfo", QString::fromStdString(infoToolName));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QString json = reply.value();
        if (json.isEmpty()) {
            std::cerr << "Error: Tool not found." << std::endl;
            return 1;
        }
        std::cout << json.toStdString() << std::endl;
        return 0;
    } else if (*categoryCmd) {
        QDBusReply<QStringList> reply = interface.call("GetToolsByCategory", QString::fromStdString(categoryName));
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QStringList tools = reply.value();
        std::cout << "Tools in category '" << categoryName << "' (" << tools.size() << "):" << std::endl;
        for (const QString& tool : tools) {
            std::cout << "  - " << tool.toStdString() << std::endl;
        }
        return 0;
    } else if (*addFavoriteCmd) {
        interface.call("AddToFavorites", QString::fromStdString(addFavoriteTool));
        std::cout << "Added to favorites: " << addFavoriteTool << std::endl;
        return 0;
    } else if (*removeFavoriteCmd) {
        interface.call("RemoveFromFavorites", QString::fromStdString(removeFavoriteTool));
        std::cout << "Removed from favorites: " << removeFavoriteTool << std::endl;
        return 0;
    } else if (*listFavoritesCmd) {
        QDBusReply<QStringList> reply = interface.call("GetFavorites");
        if (!reply.isValid()) {
            std::cerr << "Error: " << reply.error().message().toStdString() << std::endl;
            return 1;
        }
        QStringList favorites = reply.value();
        std::cout << "Favorites (" << favorites.size() << "):" << std::endl;
        for (const QString& tool : favorites) {
            std::cout << "  - " << tool.toStdString() << std::endl;
        }
        return 0;
    } else {
        std::cout << cliApp.help() << std::endl;
    }

    return 0;
}

