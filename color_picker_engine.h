/**
 * @file color_picker_engine.h
 *
 * @brief Small cross-platform color-picker engine for retrieving the cursor position and sampling RGB colors from the
 * screen.
 *
 * This is a single-header library. Define COLOR_PICKER_ENGINE_IMPLEMENTATION in exactly one translation unit before
 * including this file:
 *
 * @code
 * #define COLOR_PICKER_ENGINE_IMPLEMENTATION
 * #include "color_picker_engine.h"
 *
 * int main(void) { ... }
 * @endcode
 *
 * All other translation units should include the header without defining COLOR_PICKER_ENGINE_IMPLEMENTATION:
 *
 * @code
 * #include "color_picker_engine.h"
 * @endcode
 *
 * The implementation currently supports:
 *
 * - Windows through the Win32 API, requiring Windows Vista or later for GetPhysicalCursorPos.
 * - X11 through Xlib, requiring the Xlib development headers and library, as well as an X11 display during runtime.
 *
 * On Windows, the implementation is selected when _WIN32 is defined. On other platforms, the implementation uses X11.
 */

#if !defined(COLOR_PICKER_ENGINE_H)
#define COLOR_PICKER_ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

	/**
	 * @brief Error values returned by the Color Picker Engine.
	 *
	 * These values describe errors detected by the engine or by the platform-specific implementation.
	 *
	 * CPE_ERROR_NONE represents the absence of an error and has the special value -1.
	 *
	 * Platform-specific errors are represented by the corresponding CPE_ERROR_WIN32_ or CPE_ERROR_X11_ value.
	 * The CPE_Error.origin field identifies the subsystem that detected the error.
	 */
	enum CPE_ErrorValue
	{
		/** No error. */
		CPE_ERROR_NONE = -1,

		/**
		 * @brief An operation failed without a more specific error classification.
		 *
		 * This value is used when the library knows that an operation failed but cannot provide a more specific error
		 * value.
		 */
		CPE_ERROR_UNKNOWN,

		/** A required memory allocation failed. */
		CPE_ERROR_OUT_OF_MEMORY,

		/** A required CPE_Context pointer was NULL. */
		CPE_ERROR_CONTEXT_IS_NULL,

		/** A required CPE_Point pointer was NULL. */
		CPE_ERROR_POINT_IS_NULL,

		/** A required CPE_RGB pointer was NULL. */
		CPE_ERROR_RGB_IS_NULL,

		/** The Win32 device context could not be acquired. */
		CPE_ERROR_WIN32_CANNOT_GET_DEVICE_CONTEXT,

		/** The requested pixel could not be read through Win32. */
		CPE_ERROR_WIN32_CANNOT_GET_PIXEL,

		/** The cursor position could not be retrieved through Win32. */
		CPE_ERROR_WIN32_CANNOT_GET_CURSOR_POSITION,

		/** The X11 display could not be opened. */
		CPE_ERROR_X11_CANNOT_OPEN_DISPLAY,

		/** XQueryPointer failed to retrieve the cursor position. */
		CPE_ERROR_X11_QUERY_POINTER_FAILED,

		/** XGetImage failed to retrieve the requested screen pixel. */
		CPE_ERROR_X11_GET_IMAGE_FAILED,
	};

	/**
	 * @brief Describes an error produced by the Color Picker Engine.
	 *
	 * The @c origin field identifies the subsystem in which the error occurred. Possible origins include:
	 *
	 * - "Color Picker Engine" for errors detected directly by the library.
	 * - "Win32" for errors detected by the Windows implementation.
	 * - "X11" for errors detected by the X11 implementation.
	 *
	 * The @c value field contains the corresponding CPE_ErrorValue.
	 */
	struct CPE_Error
	{
		/** Error origin, such as "Color Picker Engine", "Win32", or "X11". */
		const char *origin;

		/** Error value describing the failure. */
		enum CPE_ErrorValue value;
	};

	/**
	 * @brief Opaque engine context containing platform-specific engine state.
	 */
	struct CPE_Context;

	/**
	 * @brief A point in screen coordinates.
	 *
	 * The coordinate system follows the native desktop coordinate system of the underlying platform.
	 *
	 * On Windows, coordinates are physical desktop coordinates and may be negative when a display is positioned to the
	 * left or above the primary display.
	 *
	 * On X11, coordinates are root-window coordinates and follow the X11 screen coordinate system.
	 */
	struct CPE_Point
	{
		/** Horizontal screen coordinate. */
		int32_t x;

		/** Vertical screen coordinate. */
		int32_t y;
	};

	/**
	 * @brief An RGB color represented by three 8-bit channels.
	 *
	 * Each channel has a range from 0 through 255.
	 */
	struct CPE_RGB
	{
		/** Red channel. */
		uint8_t r;

		/** Green channel. */
		uint8_t g;

		/** Blue channel. */
		uint8_t b;
	};

	/**
	 * @brief Packs an RGB color into a 24-bit integer value.
	 *
	 * The resulting value uses the format 0xRRGGBB. The most-significant byte of the returned uint32_t is always zero.
	 *
	 * @param rgb RGB color to convert.
	 *
	 * @return Packed 0xRRGGBB color value.
	 */
	uint32_t cpe_get_value_from_rgb(struct CPE_RGB rgb);

	/**
	 * @brief Converts a packed 0xRRGGBB value into an RGB color.
	 *
	 * Only the least-significant 24 bits of @p value are used.
	 *
	 * @param value Packed 0xRRGGBB color value.
	 *
	 * @return RGB color represented by @p value.
	 */
	struct CPE_RGB cpe_get_rgb_from_value(uint32_t value);

	/**
	 * @brief Returns a human-readable description for an error value.
	 *
	 * The returned string describes the supplied CPE_ErrorValue. It does not include platform-specific diagnostic
	 * information beyond the error category represented by the value.
	 *
	 * CPE_ERROR_UNKNOWN indicates that the library detected a failure but could not determine a more specific cause.
	 * If @p value does not correspond to an error value recognized by this version of the library, the function returns
	 * a generic "Unknown error." description.
	 *
	 * The returned string is statically allocated and must not be freed or modified by the caller.
	 *
	 * @param value Error value to describe.
	 *
	 * @return Constant string describing @p value.
	 */
	const char *cpe_get_error_description(enum CPE_ErrorValue value);

	/**
	 * @brief Creates a context containing platform-specific engine state.
	 *
	 * The returned pointer is NULL if context creation fails. If @p error is not NULL, it is set to CPE_ERROR_NONE on
	 * success or to the corresponding error value on failure.
	 *
	 * The context must be destroyed with cpe_destroy_context() when it is no longer needed.
	 *
	 * @param[out] error Optional error structure. May be NULL.
	 *
	 * @return Pointer to the newly created context, or NULL if creation fails.
	 */
	struct CPE_Context *cpe_create_context(struct CPE_Error *error);

	/**
	 * @brief Destroys a context and releases its resources.
	 *
	 * @param[in] ctx The context to destroy. May be NULL.
	 *
	 * @return NULL, allowing the caller to assign the result to its context pointer.
	 */
	struct CPE_Context *cpe_destroy_context(struct CPE_Context *ctx);

	/**
	 * @brief Retrieves the current cursor position.
	 *
	 * The returned coordinates use the native screen coordinate system of the underlying platform.
	 *
	 * On Windows, the function queries the physical cursor position.
	 *
	 * On X11, the function queries the pointer position relative to the root window.
	 *
	 * @param[in] ctx The context containing platform-specific engine state.
	 * @param[out] point Destination for the cursor position.
	 * @param[out] error Optional error structure. May be NULL.
	 *
	 * @return true on success, false on failure.
	 *
	 * If @p error is not NULL, @p error is set to CPE_ERROR_NONE at the beginning of the operation and updated to the
	 * corresponding CPE_ErrorValue if the operation fails.
	 */
	bool cpe_get_cursor_position(struct CPE_Context *ctx, struct CPE_Point *point, struct CPE_Error *error);

	/**
	 * @brief Samples the RGB color of a screen pixel.
	 *
	 * The pixel is sampled from the desktop/root screen at the specified screen coordinates.
	 *
	 * On Windows, the pixel is read from the desktop device context using the Win32 GetPixel API.
	 *
	 * On X11, a 1x1 image is retrieved from the root window using XGetImage. The pixel's red, green, and blue
	 * components are extracted using the XImage channel masks and converted to 8-bit RGB values.
	 *
	 * @param[in] ctx The context containing platform-specific engine state.
	 * @param[out] rgb Destination for the sampled RGB color.
	 * @param[in] point Screen coordinate to sample.
	 * @param[out] error Optional error structure. May be NULL.
	 *
	 * @return true on success, false on failure.
	 *
	 * If @p error is not NULL, @p error is set to CPE_ERROR_NONE at the beginning of the operation and updated to the
	 * corresponding CPE_ErrorValue if the operation fails.
	 */
	bool cpe_sample_point_rgb(struct CPE_Context *ctx, struct CPE_RGB *rgb, const struct CPE_Point *point,
	                          struct CPE_Error *error);

#ifdef __cplusplus
}
#endif

#endif

#if defined(COLOR_PICKER_ENGINE_IMPLEMENTATION)

static const char *const COLOR_PICKER_ENGINE_ERROR_ORIGIN = "Color Picker Engine";

static void error_try_set(struct CPE_Error *error, const char *origin, enum CPE_ErrorValue value)
{
	if (error == NULL)
	{
		return;
	}
	error->origin = origin;
	error->value = value;
}

uint32_t cpe_get_value_from_rgb(struct CPE_RGB rgb)
{
	return ((uint32_t)rgb.r << 16) | ((uint32_t)rgb.g << 8) | (uint32_t)rgb.b;
}

struct CPE_RGB cpe_get_rgb_from_value(uint32_t value)
{
	struct CPE_RGB rgb = {0, 0, 0};
	rgb.r = (value >> 16) & 0xff;
	rgb.g = (value >> 8) & 0xff;
	rgb.b = value & 0xff;
	return rgb;
}

const char *cpe_get_error_description(enum CPE_ErrorValue value)
{
	switch (value)
	{
	case CPE_ERROR_NONE:
		return "No error.";

	case CPE_ERROR_UNKNOWN:
		return "Unknown error.";

	case CPE_ERROR_OUT_OF_MEMORY:
		return "Requested memory allocation could not be satisfied.";

	case CPE_ERROR_CONTEXT_IS_NULL:
		return "Pointer to CPE_Context structure is NULL.";

	case CPE_ERROR_POINT_IS_NULL:
		return "Pointer to CPE_Point structure is NULL.";

	case CPE_ERROR_RGB_IS_NULL:
		return "Pointer to CPE_RGB structure is NULL.";

	case CPE_ERROR_WIN32_CANNOT_GET_DEVICE_CONTEXT:
		return "Could not acquire the Windows device context.";

	case CPE_ERROR_WIN32_CANNOT_GET_PIXEL:
		return "Could not read the pixel at the specified position.";

	case CPE_ERROR_WIN32_CANNOT_GET_CURSOR_POSITION:
		return "Could not retrieve the cursor position.";

	case CPE_ERROR_X11_CANNOT_OPEN_DISPLAY:
		return "Could not open the X11 display.";

	case CPE_ERROR_X11_QUERY_POINTER_FAILED:
		return "XQueryPointer failed.";

	case CPE_ERROR_X11_GET_IMAGE_FAILED:
		return "XGetImage failed.";

	default:
		return "Unknown error.";
	}
}

#if defined(_WIN32)

#include <stdlib.h>
#include <windows.h>

static const char *const COLOR_PICKER_WIN32_ERROR_ORIGIN = "Win32";

struct CPE_Context
{
	HDC device_context;
};

struct CPE_Context *cpe_create_context(struct CPE_Error *error)
{
	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	struct CPE_Context *ctx = malloc(sizeof *ctx);
	if (ctx == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_OUT_OF_MEMORY);
		return NULL;
	}
	*ctx = (struct CPE_Context){
	    .device_context = NULL,
	};

	ctx->device_context = GetDC(NULL);
	if (ctx->device_context == NULL)
	{
		error_try_set(error, COLOR_PICKER_WIN32_ERROR_ORIGIN, CPE_ERROR_WIN32_CANNOT_GET_DEVICE_CONTEXT);
		free(ctx);
		return NULL;
	}

	return ctx;
}

struct CPE_Context *cpe_destroy_context(struct CPE_Context *ctx)
{
	if (ctx == NULL)
	{
		return NULL;
	}

	(void)ReleaseDC(NULL, ctx->device_context);

	free(ctx);

	return NULL;
}

bool cpe_get_cursor_position(struct CPE_Context *ctx, struct CPE_Point *point, struct CPE_Error *error)
{
	(void)ctx;

	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	if (point == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_POINT_IS_NULL);
		return false;
	}

	POINT desktop_point = {0, 0};
	if (!GetPhysicalCursorPos(&desktop_point))
	{
		error_try_set(error, COLOR_PICKER_WIN32_ERROR_ORIGIN, CPE_ERROR_WIN32_CANNOT_GET_CURSOR_POSITION);
		return false;
	}

	point->x = desktop_point.x;
	point->y = desktop_point.y;

	return true;
}

bool cpe_sample_point_rgb(struct CPE_Context *ctx, struct CPE_RGB *rgb, const struct CPE_Point *point,
                          struct CPE_Error *error)
{
	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	if (ctx == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_CONTEXT_IS_NULL);
		return false;
	}

	if (rgb == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_RGB_IS_NULL);
		return false;
	}

	if (point == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_POINT_IS_NULL);
		return false;
	}

	COLORREF pixel = GetPixel(ctx->device_context, point->x, point->y);
	if (pixel == CLR_INVALID)
	{
		error_try_set(error, COLOR_PICKER_WIN32_ERROR_ORIGIN, CPE_ERROR_WIN32_CANNOT_GET_PIXEL);
		return false;
	}

	rgb->r = (uint8_t)GetRValue(pixel);
	rgb->g = (uint8_t)GetGValue(pixel);
	rgb->b = (uint8_t)GetBValue(pixel);

	return true;
}

#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>

static const char *const COLOR_PICKER_X11_ERROR_ORIGIN = "X11";

struct CPE_Context
{
	Display *display;
};

struct CPE_Context *cpe_create_context(struct CPE_Error *error)
{
	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	struct CPE_Context *ctx = malloc(sizeof *ctx);
	if (ctx == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_OUT_OF_MEMORY);
		return NULL;
	}
	*ctx = (struct CPE_Context){
	    .display = NULL,
	};

	ctx->display = XOpenDisplay(NULL);
	if (ctx->display == NULL)
	{
		error_try_set(error, COLOR_PICKER_X11_ERROR_ORIGIN, CPE_ERROR_X11_CANNOT_OPEN_DISPLAY);
		free(ctx);
		return NULL;
	}

	return ctx;
}

struct CPE_Context *cpe_destroy_context(struct CPE_Context *ctx)
{
	if (ctx == NULL)
	{
		return NULL;
	}

	(void)XCloseDisplay(ctx->display);

	free(ctx);

	return NULL;
}

static unsigned int get_shift(unsigned long mask)
{
	unsigned int shift = 0;

	while (mask != 0 && (mask & 1UL) == 0)
	{
		mask >>= 1;
		shift++;
	}

	return shift;
}

static uint8_t extract_channel(unsigned long pixel, unsigned long mask)
{
	if (mask == 0)
	{
		return 0;
	}

	unsigned int shift = get_shift(mask);
	unsigned long max = mask >> shift;
	unsigned long value = (pixel & mask) >> shift;

	return (uint8_t)((value * 255UL + max / 2UL) / max);
}

bool cpe_get_cursor_position(struct CPE_Context *ctx, struct CPE_Point *point, struct CPE_Error *error)
{
	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	if (ctx == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_CONTEXT_IS_NULL);
		return false;
	}

	if (point == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_POINT_IS_NULL);
		return false;
	}

	Window root = XDefaultRootWindow(ctx->display);

	Window root_return, child_return;
	int root_x_return, root_y_return;
	int win_x_return, win_y_return;
	unsigned int mask_return;

	Bool success = XQueryPointer(ctx->display, root, &root_return, &child_return, &root_x_return, &root_y_return,
	                             &win_x_return, &win_y_return, &mask_return);
	if (success == False)
	{
		error_try_set(error, COLOR_PICKER_X11_ERROR_ORIGIN, CPE_ERROR_X11_QUERY_POINTER_FAILED);
		return false;
	}

	point->x = root_x_return;
	point->y = root_y_return;

	return true;
}

bool cpe_sample_point_rgb(struct CPE_Context *ctx, struct CPE_RGB *rgb, const struct CPE_Point *point,
                          struct CPE_Error *error)
{
	error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_NONE);

	if (ctx == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_CONTEXT_IS_NULL);
		return false;
	}

	if (rgb == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_RGB_IS_NULL);
		return false;
	}

	if (point == NULL)
	{
		error_try_set(error, COLOR_PICKER_ENGINE_ERROR_ORIGIN, CPE_ERROR_POINT_IS_NULL);
		return false;
	}

	Window root = XDefaultRootWindow(ctx->display);

	XImage *image = XGetImage(ctx->display, root, point->x, point->y, 1, 1, AllPlanes, ZPixmap);
	if (image == NULL)
	{
		error_try_set(error, COLOR_PICKER_X11_ERROR_ORIGIN, CPE_ERROR_X11_GET_IMAGE_FAILED);
		return false;
	}

	unsigned long pixel = XGetPixel(image, 0, 0);

	rgb->r = extract_channel(pixel, image->red_mask);
	rgb->g = extract_channel(pixel, image->green_mask);
	rgb->b = extract_channel(pixel, image->blue_mask);

	XDestroyImage(image);

	return true;
}

#endif

#endif
