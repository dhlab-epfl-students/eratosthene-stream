#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "models.h"

#ifndef ERATOSTHENE_STREAM_ENGINE_H
#define ERATOSTHENE_STREAM_ENGINE_H

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};


const int MAX_FRAMES_IN_FLIGHT = 2;

class Engine {
public:
    explicit Engine(GLFWwindow *window);
    ~Engine();
    void feedVertices(std::vector<Vertex> vector);
    void drawTriangles(std::vector<uint16_t> indices);
    void drawPoints(std::vector<uint16_t> indices);
    void drawLines(std::vector<uint16_t> indices);
    void prepareRendering();
    void render();
    void waitIdle();
    void setFramebufferResized();

    void setSceneView(glm::mat4 sceneView) {
        this->sceneView = sceneView;
    }


private:
    /*
     * Class members
     * ------------------------------------------------------------------------
     */

    // Vulkan main objects
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;

    // Vulkan devices
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    // 3D drawing data
    std::vector<Vertex> vertices;
    std::vector<uint16_t> triangles;
    std::vector<uint16_t> lines;
    std::vector<uint16_t> points;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // 3D depth buffering
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    // Frame refresh concurrency
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    glm::mat4 sceneView;


    /*
     * Internal methods
     * ------------------------------------------------------------------------
     */
    void createInstance();
    void createSurface();
    void createLogicalDevice();
    void createSwapChain();
    void recreateSwapChain();
    void cleanup();
    void cleanupSwapChain();
    void setupDebugMessenger();
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImageViews();
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);
    void createDepthResources();
    void createRenderPass();
    void createGraphicsPipeline();
    void createDescriptorSetLayout();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createCommandBuffers();
    void createSyncObjects();
    void updateUniformBuffer(uint32_t currentImage);
    void drawFrame();
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    void createCommandPool();
    void createFramebuffers();

    VkShaderModule createShaderModule(const std::vector<char> &code);

};


#endif //ERATOSTHENE_STREAM_ENGINE_H
