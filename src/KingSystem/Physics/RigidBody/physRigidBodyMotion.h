#pragma once

#include <container/seadPtrArray.h>
#include <prim/seadTypedBitFlag.h>
#include <thread/seadAtomic.h>
#include <thread/seadCriticalSection.h>
#include "KingSystem/Physics/RigidBody/physMotionAccessor.h"
#include "KingSystem/Physics/RigidBody/physRigidBody.h"

class hkpMotion;

namespace ksys::phys {

class RigidBodyMotionProxy;

class RigidBodyMotion : public MotionAccessor {
    SEAD_RTTI_OVERRIDE(RigidBodyMotion, MotionAccessor)
public:
    enum class Flag {
        _1 = 1 << 0,
        _2 = 1 << 1,
        _200 = 1 << 9,
    };

    explicit RigidBodyMotion(RigidBody* body);

    void setTransform(const sead::Matrix34f& mtx, bool propagate_to_linked_motions) override;
    void setPosition(const sead::Vector3f& position, bool propagate_to_linked_motions) override;
    void getPosition(sead::Vector3f* position) override;
    hkVector4f getPosition() const;
    void getRotation(sead::Quatf* rotation) override;
    hkQuaternionf getRotation() const;
    void getTransform(sead::Matrix34f* mtx) override;

    void setCenterOfMassInLocal(const sead::Vector3f& center) override;
    void getCenterOfMassInLocal(sead::Vector3f* center) override;

    bool setLinearVelocity(const sead::Vector3f& velocity, float epsilon) override;
    bool setLinearVelocity(const hkVector4f& velocity, float epsilon) override;
    void getLinearVelocity(sead::Vector3f* velocity) override;
    bool setAngularVelocity(const sead::Vector3f& velocity, float epsilon) override;
    bool setAngularVelocity(const hkVector4f& velocity, float epsilon) override;
    void getAngularVelocity(sead::Vector3f* velocity) override;

    void setMaxLinearVelocity(float max) override;
    float getMaxLinearVelocity() override;
    void setMaxAngularVelocity(float max) override;
    float getMaxAngularVelocity() override;

    ~RigidBodyMotion() override;

    bool init(const RigidBodyInstanceParam& params, sead::Heap* heap) override;
    void getRotation(hkQuaternionf* quat) override;
    void setTimeFactor(float factor) override;
    float getTimeFactor() override;

    void freeze(bool freeze, bool preserve_velocities, bool preserve_max_impulse) override;
    void resetFrozenState() override {
        mLinearVelocity.set(0, 0, 0);
        mAngularVelocity.set(0, 0, 0);
    }

    bool applyLinearImpulse(const sead::Vector3f& impulse);
    bool applyAngularImpulse(const sead::Vector3f& impulse);
    bool applyPointImpulse(const sead::Vector3f& impulse, const sead::Vector3f& point);

    void setMass(float mass);
    float getMass() const;
    float getMassInv() const;

    void setInertiaLocal(const sead::Vector3f& inertia);
    void getInertiaLocal(sead::Vector3f* inertia) const;

    void setLinearDamping(float value);
    float getLinearDamping() const;
    void setAngularDamping(float value);
    float getAngularDamping() const;
    void setGravityFactor(float value);
    float getGravityFactor() const;

    void processUpdateFlags();
    void updateRigidBodyMotionExceptState();
    void updateRigidBodyMotionExceptStateAndVel();

    bool registerAccessor(RigidBodyMotionProxy* accessor);
    bool deregisterAccessor(RigidBodyMotionProxy* accessor);
    bool deregisterAllAccessors();
    void copyTransformToAllLinkedBodies();
    void copyMotionToAllLinkedBodies();

    static void setImpulseEpsilon(float epsilon);
    static void setMaxImpulse(float max_impulse);

private:
    hkpMotion*
    getHkBodyMotionOrLocalMotionIf(RigidBody::MotionFlag use_local_motion_condition) const {
        if (hasMotionFlagSet(use_local_motion_condition))
            return mMotion;
        return getRigidBodyMotion();
    }

    bool arePropertyChangesBlocked() const { return mBody->hasFlag(RigidBody::Flag::_80000); }

    sead::Vector3f mLinearVelocity = sead::Vector3f::zero;
    float mLinearDamping{};
    sead::Vector3f mAngularVelocity = sead::Vector3f::zero;
    float mAngularDamping{};
    sead::Vector3f mInertiaLocal = sead::Vector3f::zero;
    float mMass{};
    float mGravityFactor{};
    float mMaxImpulseCopy{};
    hkpMotion* mMotion{};
    sead::FixedPtrArray<RigidBodyMotionProxy, 8> mLinkedAccessors;
    float mWaterBuoyancyScale = 1.0f;
    float mWaterFlowEffectiveRate = 1.0f;
    float mMagneMassScalingFactor = 1.0f;
    float mFrictionScale = 1.0f;
    float mRestitutionScale = 1.0f;
    float mMaxImpulse = -1.0f;
    float mColImpulseScale = 1.0f;
    sead::Atomic<u32> _c4;
    sead::TypedBitFlag<Flag, sead::Atomic<u32>> mFlags;
    u32 _cc{};
    sead::CriticalSection mCS;
};

}  // namespace ksys::phys
