#ifndef Magnum_SceneGraph_Animable_h
#define Magnum_SceneGraph_Animable_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class Magnum::SceneGraph::Animable, alias Magnum::SceneGraph::BasicAnimable2D, Magnum::SceneGraph::BasicAnimable3D, typedef Magnum::SceneGraph::Animable2D, Magnum::SceneGraph::Animable3D, enum Magnum::SceneGraph::AnimationState
 */

#include "AbstractGroupedFeature.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Animation state

@see Animable::setState()
*/
enum class AnimationState: UnsignedByte {
    /**
     * The animation is stopped. The animation will be started from the
     * beginning when state is changed to @ref AnimationState::Running.
     */
    Stopped,

    /**
     * The animation is stopped. The animation will continue from paused
     * position when state is changed to @ref AnimationState::Running.
     */
    Paused,

    /** The animation is running. */
    Running
};

/** @debugoperator{Magnum::SceneGraph::Animable} */
Debug MAGNUM_SCENEGRAPH_EXPORT operator<<(Debug debug, AnimationState value);

/**
@brief %Animable

Adds animation feature to object. Each %Animable is part of some AnimableGroup,
which takes care of running the animations.

@section Animable-usage Usage

First thing is add Animable feature to some object and implement
animationStep(). You can do it conveniently using multiple inheritance (see
@ref scenegraph-features for introduction). Override animationStep() to
implement your animation, the function provides both absolute animation
time and time delta. Example:
@code
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class AnimableObject: public Object3D, SceneGraph::Animable3D {
    public:
        AnimableObject(Object* parent = nullptr, SceneGraph::DrawableGroup3D* group = nullptr): Object3D(parent), SceneGraph::Animable3D(*this, group) {
            setDuration(10.0f);
            // ...
        }

        void animationStep(Float time, Float delta) override {
            rotateX(15.0_degf*delta); // rotate at 15 degrees per second
        }
}
@endcode

Then add the object to your scene and some animation group. You can also use
AnimableGroup::add() and AnimableGroup::remove() instead of passing the group
in the constructor. The animation is initially in stopped state and without
repeat, see setState(), setRepeated() and setRepeatCount() for more information.
@code
Scene3D scene;
SceneGraph::AnimableGroup3D animables;

(new AnimableObject(&scene, &animables))
    ->setState(SceneGraph::AnimationState::Running);
// ...
@endcode

Animation step is performed by calling AnimableGroup::step() in your draw event
implementation. The function expects absolute time from relative to some fixed
point in the past and time delta (i.e. duration of the frame). You can use
Timeline for that, see its documentation for more information.
@code
Timeline timeline;
timeline.start();

void MyApplication::drawEvent() {
    animables.step(timeline.lastFrameTime(), timeline.lastFrameDuration());

    // ...

    timeline.nextFrame();
}
@endcode

@section Animable-performance Using animable groups to improve performance

AnimableGroup is optimized for case when no animation is running -- it just
puts itself to rest and waits until some animation changes its state to
@ref AnimationState::Running again. If you put animations which are not
pernamently running to separate group, they will not be always traversed when
calling @ref AnimableGroup::step(), saving precious frame time.

@section Animable-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Double type) you have to
use @ref Animable.hpp implementation file to avoid linker errors. See also
@ref compilation-speedup-hpp for more information.

-   @ref Animable2D, @ref AnimableGroup2D
-   @ref Animable3D, @ref AnimableGroup3D

@see @ref scenegraph, @ref BasicAnimable2D, @ref BasicAnimable3D,
    @ref Animable2D, @ref Animable3D, @ref AnimableGroup
*/
template<UnsignedInt dimensions, class T> class Animable: public AbstractGroupedFeature<dimensions, Animable<dimensions, T>, T> {
    friend class AnimableGroup<dimensions, T>;

    public:
        /**
         * @brief Constructor
         * @param object    %Object this animable belongs to
         * @param group     Group this animable belongs to
         *
         * Creates stopped non-repeating animation with infinite duration,
         * adds the feature to the object and also to group, if specified.
         * @see setDuration(), setState(), setRepeated(), AnimableGroup::add()
         */
        explicit Animable(AbstractObject<dimensions, T>& object, AnimableGroup<dimensions, T>* group = nullptr);

        ~Animable();

        /** @brief Animation duration */
        Float duration() const { return _duration; }

        /** @brief Animation state */
        AnimationState state() const { return currentState; }

        /**
         * @brief Set animation state
         * @return Reference to self (for method chaining)
         *
         * Note that changing state from @ref AnimationState::Stopped to
         * @ref AnimationState::Paused is ignored and animation remains in
         * @ref AnimationState::Stopped state. See also @ref animationStep()
         * for more information.
         * @see @ref animationStarted(), @ref animationPaused(),
         *      @ref animationResumed(), @ref animationStopped()
         */
        Animable<dimensions, T>& setState(AnimationState state);

        /**
         * @brief Whether the animation is repeated
         *
         * @see repeatCount()
         */
        bool isRepeated() const { return _repeated; }

        /**
         * @brief Enable/disable repeated animation
         * @return Reference to self (for method chaining)
         *
         * Default is `false`.
         * @see setRepeatCount()
         */
        Animable<dimensions, T>& setRepeated(bool repeated) {
            _repeated = repeated;
            return *this;
        }

        /**
         * @brief Repeat count
         *
         * @see isRepeated()
         */
        UnsignedShort repeatCount() const { return _repeatCount; }

        /**
         * @brief Set repeat count
         * @return Reference to self (for method chaining)
         *
         * Has effect only if repeated animation is enabled. `0` means
         * infinitely repeated animation. Default is `0`.
         * @see setRepeated()
         */
        Animable<dimensions, T>& setRepeatCount(UnsignedShort count) {
            _repeatCount = count;
            return *this;
        }

        /**
         * @brief Group containing this animable
         *
         * If the animable doesn't belong to any group, returns `nullptr`.
         */
        AnimableGroup<dimensions, T>* animables();
        const AnimableGroup<dimensions, T>* animables() const; /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copydoc animables()
         * @deprecated Use @ref Magnum::SceneGraph::Animable::animables() "animables()"
         *      instead.
         */
        CORRADE_DEPRECATED("use animables() instead") AnimableGroup<dimensions, T>* group() { return animables(); }

        /**
         * @copydoc animables()
         * @deprecated Use @ref Magnum::SceneGraph::Animable::animables() "animables()"
         *      instead.
         */
        CORRADE_DEPRECATED("use animables() instead") const AnimableGroup<dimensions, T>* group() const { return animables(); }
        #endif

    protected:
        /**
         * @brief Set animation duration
         * @return Reference to self (for method chaining)
         *
         * Sets duration of the animation cycle in seconds. Set to `0.0f` for
         * infinite non-repeating animation. Default is `0.0f`.
         */
        /* Protected so only animation implementer can change it */
        Animable<dimensions, T>& setDuration(Float duration) {
            _duration = duration;
            return *this;
        }

        /**
         * @brief Perform animation step
         * @param time      Time from start of the animation
         * @param delta     Time delta for current frame
         *
         * This function is periodically called from @ref AnimableGroup::step()
         * if the animation state is set to @ref AnimationState::Running. After
         * animation duration is exceeded and repeat is not enabled or repeat
         * count is exceeded, the animation state is set to @ref AnimationState::Stopped.
         *
         * If the animation is resumed from @ref AnimationState::Paused, this
         * function is called with @p time continuing from the point when it
         * was paused. If the animation is resumed from @ref AnimationState::Stopped,
         * @p time starts with zero.
         *
         * @see @ref state(), @ref duration(), @ref isRepeated(),
         *      @ref repeatCount()
         */
        virtual void animationStep(Float time, Float delta) = 0;

        /**
         * @brief Action on animation start
         *
         * Called from @ref AnimableGroup::step() when state is changed from
         * @ref AnimationState::Stopped to @ref AnimationState::Running and
         * before first @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationStarted() {}

        /**
         * @brief Action on animation pause
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * @ref AnimationState::Running to @ref AnimationState::Paused and
         * after last @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationPaused() {}

        /**
         * @brief Action on animation resume
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * @ref AnimationState::Paused to @ref AnimationState::Running and
         * before first @ref animationStep() is called.
         *
         * Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationResumed() {}

        /**
         * @brief Action on animation stop
         *
         * Called from @ref AnimableGroup::step() when state changes from
         * either @ref AnimationState::Running or @ref AnimationState::Paused
         * to @ref AnimationState::Stopped and after last @ref animationStep()
         * is called.
         *
         * You may want to use this function to properly finish the animation
         * in case the framerate is not high enough to have @ref animationStep()
         * called enough times. Default implementation does nothing.
         * @see @ref setState()
         */
        virtual void animationStopped() {}

    private:
        Float _duration;
        Float startTime, pauseTime;
        AnimationState previousState;
        AnimationState currentState;
        bool _repeated;
        UnsignedShort _repeatCount;
        UnsignedShort repeats;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief %Animable for two-dimensional scenes

Convenience alternative to <tt>%Animable<2, T></tt>. See Animable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Animable<2, T></tt> instead.
@see @ref Animable2D, @ref BasicAnimable3D
*/
template<class T> using BasicAnimable2D = Animable<2, T>;
#endif

/**
@brief %Animable for two-dimensional float scenes

@see @ref Animable3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicAnimable2D<Float> Animable2D;
#else
typedef Animable<2, Float> Animable2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief %Animable for three-dimensional scenes

Convenience alternative to <tt>%Animable<3, T></tt>. See Animable for more
information.
@note Not available on GCC < 4.7. Use <tt>%Animable<3, T></tt> instead.
@see @ref Animable3D, @ref BasicAnimable2D
*/
template<class T> using BasicAnimable3D = Animable<3, T>;
#endif

/**
@brief %Animable for three-dimensional float scenes

@see @ref Animable2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicAnimable3D<Float> Animable3D;
#else
typedef Animable<3, Float> Animable3D;
#endif

#ifdef CORRADE_TARGET_WINDOWS
extern template class MAGNUM_SCENEGRAPH_EXPORT Animable<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Animable<3, Float>;
#endif

}}

#endif
