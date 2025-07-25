/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated April 5, 2025. Replaces all prior versions.
 *
 * Copyright (c) 2013-2025, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/ShearTimeline.h>

#include <spine/Event.h>
#include <spine/Skeleton.h>

#include <spine/Bone.h>
#include <spine/BoneData.h>
#include <spine/Slot.h>
#include <spine/SlotData.h>

using namespace spine;

RTTI_IMPL(ShearTimeline, CurveTimeline2)

ShearTimeline::ShearTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline2(frameCount,
																									bezierCount),
																					 _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_ShearX << 32) | boneIndex,
						((PropertyId) Property_ShearY << 32) | boneIndex};
	setPropertyIds(ids, 2);
}

ShearTimeline::~ShearTimeline() {
}

void ShearTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
						  MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);

	Bone *bone = skeleton._bones[_boneIndex];
	if (!bone->_active) return;

	if (time < _frames[0]) {
		switch (blend) {
			case MixBlend_Setup:
				bone->_shearX = bone->_data._shearX;
				bone->_shearY = bone->_data._shearY;
				return;
			case MixBlend_First:
				bone->_shearX += (bone->_data._shearX - bone->_shearX) * alpha;
				bone->_shearY += (bone->_data._shearY - bone->_shearY) * alpha;
			default: {
			}
		}
		return;
	}

	float x, y;
	int i = Animation::search(_frames, time, CurveTimeline2::ENTRIES);
	int curveType = (int) _curves[i / CurveTimeline2::ENTRIES];
	switch (curveType) {
		case CurveTimeline2::LINEAR: {
			float before = _frames[i];
			x = _frames[i + CurveTimeline2::VALUE1];
			y = _frames[i + CurveTimeline2::VALUE2];
			float t = (time - before) / (_frames[i + CurveTimeline2::ENTRIES] - before);
			x += (_frames[i + CurveTimeline2::ENTRIES + CurveTimeline2::VALUE1] - x) * t;
			y += (_frames[i + CurveTimeline2::ENTRIES + CurveTimeline2::VALUE2] - y) * t;
			break;
		}
		case CurveTimeline2::STEPPED: {
			x = _frames[i + CurveTimeline2::VALUE1];
			y = _frames[i + CurveTimeline2::VALUE2];
			break;
		}
		default: {
			x = getBezierValue(time, i, CurveTimeline2::VALUE1, curveType - CurveTimeline2::BEZIER);
			y = getBezierValue(time, i, CurveTimeline2::VALUE2,
							   curveType + CurveTimeline2::BEZIER_SIZE - CurveTimeline2::BEZIER);
		}
	}

	switch (blend) {
		case MixBlend_Setup:
			bone->_shearX = bone->_data._shearX + x * alpha;
			bone->_shearY = bone->_data._shearY + y * alpha;
			break;
		case MixBlend_First:
		case MixBlend_Replace:
			bone->_shearX += (bone->_data._shearX + x - bone->_shearX) * alpha;
			bone->_shearY += (bone->_data._shearY + y - bone->_shearY) * alpha;
			break;
		case MixBlend_Add:
			bone->_shearX += x * alpha;
			bone->_shearY += y * alpha;
	}
}

RTTI_IMPL(ShearXTimeline, CurveTimeline1)

ShearXTimeline::ShearXTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline1(frameCount,
																									  bezierCount),
																					   _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_ShearX << 32) | boneIndex};
	setPropertyIds(ids, 1);
}

ShearXTimeline::~ShearXTimeline() {
}

void ShearXTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
						   MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);

	Bone *bone = skeleton._bones[_boneIndex];
	if (bone->_active) bone->_shearX = getRelativeValue(time, alpha, blend, bone->_shearX, bone->_data._shearX);
}

RTTI_IMPL(ShearYTimeline, CurveTimeline1)

ShearYTimeline::ShearYTimeline(size_t frameCount, size_t bezierCount, int boneIndex) : CurveTimeline1(frameCount,
																									  bezierCount),
																					   _boneIndex(boneIndex) {
	PropertyId ids[] = {((PropertyId) Property_ShearX << 32) | boneIndex};
	setPropertyIds(ids, 1);
}

ShearYTimeline::~ShearYTimeline() {
}

void ShearYTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
						   MixBlend blend, MixDirection direction) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);

	Bone *bone = skeleton._bones[_boneIndex];
	if (bone->_active) bone->_shearY = getRelativeValue(time, alpha, blend, bone->_shearY, bone->_data._shearY);
}
