pluginManagement {
    repositories {
        google {
            content {
                includeGroupByRegex("com\\.android.*")
                includeGroupByRegex("com\\.google.*")
                includeGroupByRegex("androidx.*")
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "1.0.0"
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.PREFER_PROJECT)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "quickjs-kt"

enableFeaturePreview("TYPESAFE_PROJECT_ACCESSORS")

include(":quickjs")
// ponytail: оставлен только :quickjs. Конвертеры/samples/benchmark/integration-test
// убраны — не используются и ломали конфигурацию composite build (linkDebugTestMingwX64).
// Вернуть include при необходимости.
