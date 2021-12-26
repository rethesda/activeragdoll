#pragma once

#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Utilities/Collide/TriggerVolume/hkpTriggerVolume.h>
#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/LimitedHinge/hkpLimitedHingeConstraintData.h>
#include <Physics/Dynamics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>
#include <Physics/Dynamics/World/Extensions/hkpWorldExtension.h>
#include <Physics/Dynamics/World/Simulation/hkpSimulation.h>
#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxyListener.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyListener.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics/Collide/Query/Collector/PointCollector/hkpAllCdPointCollector.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterState.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>

#include "skse64_common/Relocation.h"
#include "skse64/PapyrusVM.h"
#include "skse64/GameReferences.h"

#include "RE/havok.h"
#include "RE/misc.h"
#include "havok_ref_ptr.h"


struct hkbWorldFromModelModeData
{
	hkInt16 poseMatchingBone0; // 00
	hkInt16 poseMatchingBone1; // 02
	hkInt16 poseMatchingBone2; // 04
	UInt8 mode; // 06
};

struct hkbRagdollDriver : hkReferencedObject
{
	hkReal ragdollBlendOutTime; // 10
	hkbWorldFromModelModeData worldFromModelModeData; // 14
	hkBool autoAddRemoveRagdollToWorld; // 1C
	hkBool useAsynchronousStepping; // 1D
	hkQsTransform lastWorldFromModel; // 20
	hkbWorldFromModelModeData worldFromModelModeDataInternal; // 50
	hkArray<hkBool32> reportingWhenKeyframed; // 58 - Indexed by (boneIdx >> 5), and then you >> (boneIdx & 0x1F) & 1 to extract the specific bit
	UInt64 unk68; // numRagdollBones?
	hkPointerMap<hkReferencedObject *, hkBool32> attachedRigidBodyToIndexMap; // 70 - maybe
	struct hkbCharacter *character; // 80
	hkaRagdollInstance *ragdoll; // 88
	hkQsTransform *ragdollPoseWS; // 90
	hkaRagdollRigidBodyController *ragdollController; // 98
	hkQsTransform *ragdollPoseHiResLocal; // A0 - maybe
	hkQsTransform *lastPoseLocal; // A8
	SInt32 lastNumPoseLocal; // B0
	float lastFrameRigidBodyOnFraction; // B4
	float lastFramePoweredOnFraction; // B8
	float timeRigidBodyControllerActive; // BC
	float ragdollBlendOutTimeElapsed; // C0
	hkBool canAddRagdollToWorld; // C4
	hkBool shouldReinitializeRagdollController; // C5
	hkBool isEnabled; // C6
	hkBool isPoweredControllerEnabled; // C7
	hkBool isRigidBodyControllerEnabled; // C8
	hkBool wasRigidBodyControllerEnabledLastFrame; // C9
	hkBool ragdollPoseWasUsed; // CA
	hkBool allBonesKeyframed; // CB
};
static_assert(offsetof(hkbRagdollDriver, character) == 0x80);
static_assert(sizeof(hkbRagdollDriver) == 0xD0);

struct hkbCharacterSetup : hkReferencedObject
{
	hkArray<hkRefPtr<hkaSkeletonMapper>> m_retargetingSkeletonMappers; // 10
	hkRefPtr<hkaSkeleton> m_animationSkeleton; // 20
	hkRefPtr<hkaSkeletonMapper> m_ragdollToAnimationSkeletonMapper; // 28
	hkRefPtr<hkaSkeletonMapper> m_animationToRagdollSkeletonMapper; // 30
	hkRefPtr<struct hkbAnimationBindingSet> m_animationBindingSet; // 38
	hkRefPtr<struct hkbCharacterData> m_data; // 40
	UInt64 unk48; // probably either m_unscaledAnimationSkeleton or m_mirroredSkeleton
	hkRefPtr<struct hkbSymbolIdMap> m_characterPropertyIdMap; // 50
	mutable hkCriticalSection *m_criticalSection; // 58
};

struct hkbCharacter : hkReferencedObject
{
	// members
	hkArray<hkbCharacter*>      nearbyCharacters;           // 10
	std::int16_t                currentLOD;                 // 20
	std::int16_t                numTracksInLOD;             // 22
	std::uint32_t               pad24;                      // 24
	hkStringPtr                 name;                       // 28
	hkRefPtr<hkbRagdollDriver>  ragdollDriver;              // 30
	hkRefVariant                characterControllerDriver;  // 38
	hkRefPtr<struct hkbFootIkDriver> footIkDriver;               // 40
	hkRefPtr<struct hkbHandIkDriver> handIkDriver;               // 48
	hkRefPtr<hkbCharacterSetup> setup;                      // 50
	hkRefPtr<struct hkbBehaviorGraph>  behaviorGraph;              // 58
	hkRefPtr<struct hkbProjectData>    projectData;                // 60
	hkRefPtr<struct hkbAnimationBindingSet> animationBindingSet;        // 68
	hkRefVariant                raycastInterface;           // 70
	hkRefVariant                world;                      // 78
	hkRefVariant                eventQueue;                 // 80
	hkRefVariant                worldFromModel;             // 88
	const void**                poseLocal;                  // 90 - hkSimpleArray<hkRefVariant>
	std::int32_t                numPoseLocal;               // 98
	bool                        deleteWorldFromModel;       // 9C
	bool                        deletePoseLocal;            // 9D
	std::uint16_t               pad9E;                      // 9E
};
static_assert(offsetof(hkbCharacter, behaviorGraph) == 0x58);
static_assert(sizeof(hkbCharacter) == 0xA0);

struct hkbContext
{
	hkbCharacter *character; // 00
	UInt64 unk08;
	UInt64 unk10;
	UInt64 unk18;
	UInt64 unk20;
	UInt64 unk28;
	bool unk30;
	ahkpWorld *world; // 38
	UInt64 unk40;
	UInt64 unk48;
};
static_assert(offsetof(hkbContext, world) == 0x38);


struct hkbBindable : hkReferencedObject
{
	hkRefPtr<struct hkbVariableBindingSet> variableBindingSet;  // 10
	hkArray<hkRefVariant>           cachedBindables;     // 18
	bool                            areBindablesCached;  // 28
	std::uint8_t                    pad29;               // 29
	std::uint16_t                   pad2A;               // 2A
	std::uint32_t                   pad2C;               // 2C
};
static_assert(sizeof(hkbBindable) == 0x30);

struct hkbNode : hkbBindable
{
	UInt64 userData;    // 30
	hkStringPtr                                name;        // 38
	std::uint16_t                              id;          // 40
	std::uint8_t                               cloneState;  // 42
	std::uint8_t                               pad43;       // 43
	std::uint32_t                              pad44;       // 44
};
static_assert(offsetof(hkbNode, userData) == 0x30);
static_assert(sizeof(hkbNode) == 0x48);

struct hkbGenerator : hkbNode {};
struct hkbBehaviorGraph : hkbGenerator {};

struct bhkCharacterController : NiRefObject
{
	UInt8 unk10[0x70 - 0x10];
	hkVector4                                        forwardVec;                 // 070
	hkStepInfo                                       stepInfo;                   // 080
	hkVector4                                        outVelocity;                // 090
	hkVector4                                        initialVelocity;            // 0A0
	hkVector4                                        velocityMod;                // 0B0
	hkVector4                                        direction;                  // 0C0
	hkVector4                                        rotCenter;                  // 0D0
	hkVector4                                        pushDelta;                  // 0E0
	hkVector4                                        fakeSupportStart;           // 0F0
	hkVector4                                        up;                         // 100
	hkVector4                                        supportNorm;                // 110
	UInt8                                          collisionBound[0x150 - 0x120];             // 120
	UInt8                                          bumperCollisionBound[0x180 - 0x150];       // 150
	std::uint64_t                                    unk180;                     // 180
	std::uint64_t                                    unk188;                     // 188
	struct bhkICharOrientationController*                   orientationCtrl;            // 190
	std::uint64_t                                    pad198;                     // 198
	hkpSurfaceInfo                                   surfaceInfo;                // 1A0
	hkpCharacterContext                              context;                    // 1E0
	UInt32 flags;                      // 218
	hkpCharacterStateType                            wantState;                  // 218
	float                                            velocityTime;               // 220
	float                                            rotMod;                     // 224
	float                                            rotModTime;                 // 228
	float                                            calculatePitchTimer;        // 22C
	float                                            acrobatics;                 // 230
	float                                            center;                     // 234
	float                                            waterHeight;                // 238
	float                                            jumpHeight;                 // 23C
	float                                            fallStartHeight;            // 240
	float                                            fallTime;                   // 244
	float                                            gravity;                    // 248
	float                                            pitchAngle;                 // 24C
	float                                            rollAngle;                  // 250
	float                                            pitchMult;                  // 254
	float                                            scale;                      // 258
	float                                            swimFloatHeight;            // 25C
	float                                            actorHeight;                // 260
	float                                            speedPct;                   // 264
	std::uint32_t                                    pushCount;                  // 268
	std::uint32_t                                    unk26C;                     // 26C
	std::uint64_t                                    unk270;                     // 270
	std::uint64_t                                    unk278;                     // 278
	NiPointer<bhkShape>                              shapes[2];                  // 280
	std::uint64_t                                    unk290;                     // 290
	std::uint64_t                                    unk298;                     // 298
	std::uint64_t                                    unk2A0;                     // 2A0
	std::uint64_t                                    unk2A8;                     // 2A8
	hkRefPtr<hkpRigidBody>                           supportBody;                // 2B0
	float                                            bumpedForce;                // 2B8
	std::uint32_t                                    pad2BC;                     // 2BC
	hkRefPtr<hkpRigidBody>                           bumpedBody;                 // 2C0
	hkRefPtr<hkpRigidBody>                           bumpedCharCollisionObject;  // 2C8
	UInt8                     unk2D0[0x300 - 0x2D0];                     // 2D0
	std::uint64_t                                    unk300;                     // 300
	std::uint64_t                                    unk308;                     // 308
	std::uint64_t                                    unk310;                     // 310
	std::uint64_t                                    unk318;                     // 318
	std::uint64_t                                    unk320;                     // 320
	std::uint64_t                                    unk328;                     // 328
};
static_assert(offsetof(bhkCharacterController, context) == 0x1E0);
static_assert(sizeof(bhkCharacterController) == 0x330);

class bhkCharacterPointCollector : public hkpAllCdPointCollector
{
	UInt64 unk220;  // 220
	UInt64 unk228;  // 228
	UInt64 unk230;  // 230
	UInt64 unk238;  // 238
};
static_assert(sizeof(bhkCharacterPointCollector) == 0x240);

struct bhkCharacterProxy : bhkSerializable
{
	hkpCharacterProxy *characterProxy; // 10
	UInt64 unk18;
	bhkCharacterPointCollector ignoredCollisionStartCollector; // 020
};

struct bhkCharacterRigidBody : bhkSerializable
{
	hkpCharacterRigidBody *characterRigidBody; // 10
	UInt64 unk18;
	bhkRigidBody *rigidBody; // 20
	NiAVObject *unk28; // 28 - MarkerX ??
	bhkCharacterPointCollector ignoredCollisionStartCollector;  // 30
};
static_assert(offsetof(bhkCharacterRigidBody, ignoredCollisionStartCollector) == 0x30);

struct bhkCharProxyController :
	hkpCharacterProxyListener, // 000
	bhkCharacterController // 010
{
	bhkCharacterProxy proxy; // 340
};
static_assert(offsetof(bhkCharProxyController, proxy) == 0x340);

struct bhkCharRigidBodyController :
	bhkCharacterController, // 00
	hkpCharacterRigidBodyListener // 330
{
	bhkCharacterRigidBody characterRigidBody; // 340
};
static_assert(offsetof(bhkCharRigidBodyController, characterRigidBody) == 0x340);

struct BShkbAnimationGraph
{
	void *vtbl; // 00
	UInt8 unk08[0xC0 - 0x08];
	hkbCharacter character; // C0
	UInt8 unk160[0x208 - 0x160];
	hkbBehaviorGraph *behaviorGraph; // 208
	Actor *holder; // 210
	NiNode *rootNode; // 218
	UInt8 unk220[0x238 - 0x220];
	bhkWorld *world; // 238
	// more...
};
static_assert(offsetof(BShkbAnimationGraph, character) == 0xC0);
static_assert(offsetof(BShkbAnimationGraph, holder) == 0x210);

class BSAnimationGraphManager :
	BSTEventSink<BSAnimationGraphEvent>, // 00
	BSIntrusiveRefCounted // 08
{
public:
	UInt8 unk10[0x40 - 0x10];
	BSTSmallArray<BSTSmartPointer<BShkbAnimationGraph>> graphs; // 40
	UInt8 unk58[0x98 - 0x58];
	SimpleLock updateLock; // 98
	SimpleLock dependentManagerLock; // A0
	UInt32 activeGraph; // A8
	UInt32 generateDepth; // A8
};
static_assert(offsetof(BSAnimationGraphManager, graphs) == 0x40);
static_assert(offsetof(BSAnimationGraphManager, updateLock) == 0x98);

struct hkbPoweredRagdollControlData
{
	HK_ALIGN16(hkReal m_maxForce) = 50.f; // 00
	hkReal m_tau = 0.8f; // 04
	hkReal m_damping = 1.f; // 08
	hkReal m_proportionalRecoveryVelocity = 2.f; // 0C
	hkReal m_constantRecoveryVelocity = 1.f; // 10
};

struct hkbGeneratorOutput
{
	enum class StandardTracks
	{
		TRACK_WORLD_FROM_MODEL, // 00
		TRACK_EXTRACTED_MOTION, // 01
		TRACK_POSE, // 02
		TRACK_FLOAT_SLOTS, // 03
		TRACK_RIGID_BODY_RAGDOLL_CONTROLS, // 04
		TRACK_RIGID_BODY_RAGDOLL_BLEND_TIME, // 05
		TRACK_POWERED_RAGDOLL_CONTROLS, // 06
		TRACK_POWERED_RAGDOLL_WORLD_FROM_MODEL_MODE, // 07
		TRACK_KEYFRAMED_RAGDOLL_BONES, // 08
		TRACK_KEYFRAME_TARGETS, // 09
		TRACK_ANIMATION_BLEND_FRACTION, // 0A
		TRACK_ATTRIBUTES, // 0B
		TRACK_FOOT_IK_CONTROLS, // 0C
		TRACK_CHARACTER_CONTROLLER_CONTROLS, // 0D
		TRACK_HAND_IK_CONTROLS_0, // 0E
		TRACK_HAND_IK_CONTROLS_1, // 0F
		TRACK_HAND_IK_CONTROLS_2, // 10
		TRACK_HAND_IK_CONTROLS_3, // 11
		TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_0, // 12
		TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_1, // 13
		TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_2, // 14
		TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_3, // 15
		TRACK_DOCKING_CONTROLS, // 16
		TRACK_AI_CONTROL_CONTROLS_BLENDABLE, // 17
		TRACK_AI_CONTROL_CONTROLS_NON_BLENDABLE, // 18
		NUM_STANDARD_TRACKS, // 19
	};

	enum class TrackTypes
	{
		TRACK_TYPE_REAL, // 0
		TRACK_TYPE_QSTRANSFORM, // 1
		TRACK_TYPE_BINARY, // 2
	};

	enum class TrackFlags
	{
		TRACK_FLAG_ADDITIVE_POSE = 1,
		TRACK_FLAG_PALETTE = 1 << 1,
		TRACK_FLAG_SPARSE = 1 << 2,
	};

	struct TrackHeader
	{
		hkInt16 m_capacity; // 00
		hkInt16 m_numData; // 02
		hkInt16 m_dataOffset; // 04
		hkInt16 m_elementSizeBytes; // 06
		hkReal m_onFraction; // 08
		hkFlags<TrackFlags, hkInt8> m_flags; // 0C
		hkEnum<TrackTypes, hkInt8> m_type; // 0D
	};
	static_assert(sizeof(TrackHeader) == 0x10);

	struct TrackMasterHeader
	{
		hkInt32 m_numBytes; // 00
		hkInt32 m_numTracks; // 04
		hkInt8 m_unused[8]; // 08
	};

	struct Tracks
	{
		struct TrackMasterHeader m_masterHeader; // 00
		struct TrackHeader m_trackHeaders[1]; // 10
	};

	struct Track
	{
		TrackHeader* m_header; // 00
		hkReal* m_data; // 08
	};

	struct Tracks* m_tracks; // 00
	bool m_deleteTracks; // 08
};

inline hkReal * Track_getData(hkbGeneratorOutput &output, hkbGeneratorOutput::TrackHeader &header) {
	return reinterpret_cast<hkReal*>(reinterpret_cast<char*>(output.m_tracks) + header.m_dataOffset);
}

inline hkInt8* Track_getIndices(hkbGeneratorOutput &output, hkbGeneratorOutput::TrackHeader &header) {
	// must be sparse or pallette track
	int numDataBytes = HK_NEXT_MULTIPLE_OF(16, header.m_elementSizeBytes * header.m_capacity);
	return reinterpret_cast<hkInt8*>(Track_getData(output, header)) + numDataBytes;
}

typedef bool(*_IAnimationGraphManagerHolder_GetAnimationGraphManagerImpl)(IAnimationGraphManagerHolder *_this, BSTSmartPointer<BSAnimationGraphManager>& a_out);
inline bool GetAnimationGraphManager(Actor *actor, BSTSmartPointer<BSAnimationGraphManager> &out) {
	IAnimationGraphManagerHolder *animGraphManagerHolder = &actor->animGraphHolder;
	UInt64 *vtbl = *((UInt64 **)animGraphManagerHolder);
	return ((_IAnimationGraphManagerHolder_GetAnimationGraphManagerImpl)(vtbl[0x02]))(animGraphManagerHolder, out);
}


bhkCharRigidBodyController * GetCharRigidBodyController(Actor *actor);
bhkCharProxyController * GetCharProxyController(Actor *actor);
Actor * GetActorFromRagdollDriver(hkbRagdollDriver *driver);
