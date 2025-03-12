/****************************************
 * FILE: Engine/Include/ECS/ECSCore.inl
 ****************************************/
// 这个文件只会被“预处理器展开”到 ECSCore.hpp 里
// 不需要 #pragma once 或 #include 其他 ECSCore 的文件
// 需要时可以包含其他工具类、头文件，但一般不包含 .cpp

#include <cstddef>
#include <memory>

// =========== 组件相关 =============
// template <typename T, size_t N>
// void ECSCore::registerComp() {
//     mCompManager->registerComp<T, N>();
// }

// template <typename T>
// void ECSCore::addComp(Entity entity, const T& component) {
//     mCompManager->addComp<T>(entity, component);
//     auto signature = mEntityManager->getSignature(entity);
//     signature.set(getCompTypeIndex<T>(), true);
//     mEntityManager->setSignature(entity, signature);
//     // mSystemManager->onEntitySignatureChanged(entity, signature);
// }

// template <typename T>
// void ECSCore::removeComp(Entity entity) {
//     mCompManager->removeComp<T>(entity);
//     auto signature = mEntityManager->getSignature(entity);
//     signature.set(getCompTypeIndex<T>(), false);
//     mEntityManager->setSignature(entity, signature);
//     // mSystemManager->onEntitySignatureChanged(entity, signature);
// }

// template <typename T>
// auto ECSCore::readComp(Entity entity) {
//     return mCompManager->readComp<T>(entity);
// }

// template <typename T>
// auto ECSCore::writeComp(Entity entity) {
//     return mCompManager->writeComp<T>(entity);
// }

// template <typename T>
// T& ECSCore::getComp(Entity entity) {
//     return mCompManager->getComp<T>(entity);
// }

// template <typename T>
// bool ECSCore::hasComp(Entity entity) {
//     return mCompManager->hasComp<T>(entity);
// }

// // =========== 系统相关 =============
// template <typename T>
// std::shared_ptr<T> ECSCore::registerSystem() {
//     return mSystemManager->registerSystem<T>(shared_from_this());
// }

// template <typename T>
// void ECSCore::setSystemSignature(Signature signature) {
//     mSystemManager->setSignature<T>(signature);
// }

// template <typename T>
// std::shared_ptr<T> ECSCore::getSystem() {
//     return mSystemManager->getSystem<T>();
// }

// // =========== 帮助函数 =============
// template <typename T>
// size_t ECSCore::getCompTypeIndex() const {
//     static const size_t typeIndex = mCompTypeCounter++;
//     return typeIndex;
// }
