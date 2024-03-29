///	
/// File "gwindow.h"
/// Simple graphic window, based on Xlib primitives
///

#ifndef _GWINDOW_H
#define _GWINDOW_H

// Classes for simple 2-dimensional objects
#include "R2Graph/R2Graph.h"

// include the X library headers
extern "C" {

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

}

//===============================

///
/// class ListHeader. Implements base for L2-List node
/// and thus the data structure.
///
class ListHeader {
public:
    ListHeader*     next;
    ListHeader*     prev;

public:
    ListHeader():
        next(0),
        prev(0)
    {
    }

    ListHeader(ListHeader* n, ListHeader* p):
        next(n),
        prev(p)
    {
    }

    ListHeader(const ListHeader& h):
        next(h.next),
        prev(h.prev)
    {
    }

    ~ListHeader() {}
	///
	/// Assign node h to this node.
    ListHeader& operator=(const ListHeader& h) {
        next = h.next;
        prev = h.prev;
        return *this;
    }

	/// Append node to list
    void link(ListHeader& h) {
        next = &h;
        h.prev = this;
    }
};

///
/// Wrapper class for X Fonts.
///
class FontDescriptor: public ListHeader {
public:
    Font font_id;
    XFontStruct* font_struct;

    FontDescriptor(Font id, XFontStruct* fstr):
        ListHeader(),
        font_id(id),
        font_struct(fstr)
    {}
};


const int DEFAULT_BORDER_WIDTH = 2;

///
/// GWindow main class.
/// Impelents basic GUI routines, based on Xlib interfaces
///
class GWindow: public ListHeader {
public:
    // Xlib objects:
    // Display and screen are the same for all windows
    static Display*     m_Display;
    static int          m_Screen;
    static Atom         m_WMProtocolsAtom;
    static Atom         m_WMDeleteWindowAtom;

    Window   m_Window;
    Pixmap   m_Pixmap;
    GC       m_GC;

    /// Coordinates in window
    I2Point     m_WindowPosition;   ///< Window position in screen coord
    I2Rectangle m_IWinRect; 		///< Window rectangle in (local) pixel coordinates
    R2Rectangle m_RWinRect; 		///< Window rectangle in inner (real) coordinates

    I2Point m_ICurPos; ///< Current position, integer
    R2Point m_RCurPos; ///< Current psition, real

    double m_xcoeff;  ///< Optimization: Coeff. for real->integer conversion
    double m_ycoeff;

    char   m_WindowTitle[128];

    // Window state
    bool        m_WindowCreated;

    // Static members:
    // List of all windows
    static int          m_NumWindows;
    static int          m_NumCreatedWindows;
    static ListHeader   m_WindowList;
    static ListHeader   m_FontList;

protected:

    // Background, foreground
    unsigned long       m_bgPixel;
    unsigned long       m_fgPixel;
    const char*         m_bgColorName;
    const char*         m_fgColorName;

    // Border width
    int m_BorderWidth;

    // Clip rectangle
    XRectangle          m_ClipRectangle;
    bool                m_BeginExposeSeries;

public:
	
	/// Basic GWindow constructor
    GWindow();

	/// 
	/// \brief GWindow with specified place and size on screen and title
	/// \param frameRect Rectangle where window will be placed
	/// \param title Window title
	///
    GWindow(const I2Rectangle& frameRect, const char *title = 0);

	/// \brief GWindow with screen place, chosen real coordinate system and title
	/// \param frameRect Specifies where screen will be placed and what size it will have
	/// \param coordRect Specifies real coordinates in window
	///
    GWindow(
        const I2Rectangle& frameRect, 
        const R2Rectangle& coordRect,
        const char *title = 0
    );
    virtual ~GWindow();

	///
	/// \brief create X window
    /// The method uses the m_IWinRect member to
    /// define the position and size of the window, and the
    /// m_WindowTitle member to set the window title.
    /// All parameters have their default values, so the method can
    /// be called without any parameters: createWindow()
	/// \param parentWindow parent window
	/// \param borderWidth width of the border
	/// \param wndClass X Class of the window
	/// \param visual ???
	/// \param attributesValueMask Specifies X window attribures
	/// \param attributes X attributes too
	///
    void createWindow(
        GWindow* parentWindow = 0,              // parent window
        int borderWidth = DEFAULT_BORDER_WIDTH, // border width
        unsigned int wndClass = InputOutput,    // or InputOnly, CopyFromParent
        Visual* visual = CopyFromParent,        //
        unsigned long attributesValueMask = 0,  // which attributes are defined
        XSetWindowAttributes* attributes = 0    // attributes structure
    );

	///
	/// \brief create X window
	/// \param frameRect specifies position and size of the window
	/// \param title title of window
	/// \param parentWindow Parent window
	/// \param borderWidth width of window's border
	///
    void createWindow(
        const I2Rectangle& frameRect, 
        const char *title = 0,
        GWindow* parentWindow = 0,
        int borderWidth = DEFAULT_BORDER_WIDTH
    );
	
	///
	/// \brief create X window
	/// \param frameRect position and size of the window
	/// \param coordRect specifies real coordinates in window
	/// \param title title of window
	/// \param parentWindow Parent window
	///
    void createWindow(
        const I2Rectangle& frameRect, 
        const R2Rectangle& coordRect,
        const char *title = 0,
        GWindow* parentWindow = 0,
        int borderWidth = DEFAULT_BORDER_WIDTH
    );

	/// \brief Connect to X Server. Always use this method before using GWindow.
	/// \return false if attempt was unsuccessful.
    static bool initX();
	/// Disconnect from X Server. Always use this method when your program finishes
    static void closeX();
	/// Returns screen width (in pixels?)
    static int  screenMaxX();
	/// Returns screen height (in pixels?)
    static int  screenMaxY();
	/// Destroy all X Fonts;
    static void releaseFonts();

private:
    static GWindow* findWindow(Window w);

    static FontDescriptor* findFont(Font fontID);
    static void removeFontDescriptor(FontDescriptor* fd);
    static void addFontDescriptor(
        Font fontID, XFontStruct* fontStructure
    );

public:
	/// draw frame
    void drawFrame();

	/// Set real coordinate system in window by coordinate's constraints
    void setCoordinates(double xmin, double ymin, double xmax, double ymax);

	/// Set real coordinate system in window by R2Rectangle
    void setCoordinates(const R2Rectangle& coordRect);

	/// Set real coordinate system in window by two points
    void setCoordinates(const R2Point& leftBottom, const R2Point& rightTop);

    double getXMin() const { return m_RWinRect.getXMin(); }
    double getXMax() const { return m_RWinRect.getXMax(); }
    double getYMin() const { return m_RWinRect.getYMin(); }
    double getYMax() const { return m_RWinRect.getYMax(); }

	/// draw coordinate axes
    void drawAxes(
        const char *axesColorName = 0,
        bool drawGrid = false,
        const char *gridColorName = 0,
        bool offscreen = false
    );

	/// Return window rectangle with integer sizes
    I2Rectangle getWindowRect() const { return m_IWinRect; }

	/// Return window rectangle with real coordinate
    R2Rectangle getCoordRect() const  { return m_RWinRect; }

	/// Move brush pointer to specified point in integer coordinates
    void moveTo(const I2Point& p);
	
	/// Move brush pointer to specified point in real coordinates
    void moveTo(const R2Point& p);

	/// Move brush pointer to specified point in integer coordinates
    void moveTo(int x, int y);

	/// Move brush pointer to specified point in real coordinates
    void moveTo(double x, double y);

	/// ???
    void moveRel(const I2Vector& p);
    void moveRel(const R2Vector& p);
    void moveRel(int x, int y);
    void moveRel(double x, double y);

    void drawLineTo(const I2Point& p, bool offscreen = false);
    void drawLineTo(const R2Point& p, bool offscreen = false);
    void drawLineTo(int x, int y, bool offscreen = false);
    void drawLineTo(double x, double y, bool offscreen = false);

    void drawLineRel(const I2Vector& p, bool offscreen = false);
    void drawLineRel(const R2Vector& p, bool offscreen = false);
    void drawLineRel(int x, int y, bool offscreen = false);
    void drawLineRel(double x, double y, bool offscreen = false);

    void drawLine(const I2Point& p1, const I2Point& p2, bool offscreen = false);
    void drawLine(const I2Point& p,  const I2Vector& v, bool offscreen = false);
    void drawLine(int x1, int y1, int x2, int y2, bool offscreen = false);

    void drawLine(const R2Point& p1, const R2Point& p2, bool offscreen = false);
    void drawLine(const R2Point& p,  const R2Vector& v, bool offscreen = false);
    void drawLine(double x1, double y1, double x2, double y2, bool offscreen = false);

    void drawEllipse(const I2Rectangle&, bool offscreen = false);
    void drawEllipse(const R2Rectangle&, bool offscreen = false);
    void drawCircle(const I2Point& center, int radius, bool offscreen = false);
    void drawCircle(const R2Point& center, double radius, bool offscreen = false);

    void drawString(int x, int y, const char *str, int len = (-1), bool offscreen = false);
    void drawString(const I2Point& p, const char *str, int len = (-1), bool offscreen = false);
    void drawString(const R2Point& p, const char *str, int len = (-1), bool offscreen = false);
    void drawString(double x, double y, const char *str, int len = (-1), bool offscreen = false);
	
	/// Set background color name
    /// \warning The method should be called before "createWindow"
    void setBgColorName(const char* colorName);
    void setFgColorName(const char* colorName);

	/// Set line properties for lines
    void setLineAttributes(
        unsigned int line_width, int line_style, 
        int cap_style, int join_style
    );

    void setLineWidth(unsigned int line_width);

	/// Make X color
    unsigned long allocateColor(const char *colorName);

	/// Set background color
    void setBackground(unsigned long bg);
    void setBackground(const char *colorName);

	/// Set foreground color
    void setForeground(unsigned long fg);
    void setForeground(const char *colorName);

    unsigned long getBackground() const { return m_bgPixel; }
    unsigned long getForeground() const { return m_fgPixel; }

	// filling drawing methods
    void fillRectangle(const I2Rectangle&, bool offscreen = false);
    void fillRectangle(const R2Rectangle&, bool offscreen = false);

    void fillPolygon(const R2Point* points, int numPoints, bool offscreen = false);
    void fillPolygon(const I2Point* points, int numPoints, bool offscreen = false);

    void fillEllipse(const I2Rectangle&, bool offscreen = false);
    void fillEllipse(const R2Rectangle&, bool offscreen = false);

	/// Force X to yield Expose event with all window as redraw area. 
	/// onExpose() method will be called.
    void redraw();

	/// Force X to yield Expose event with specified
	/// real rectangle as redraw area.
	/// onExpose() method will be called
    void redrawRectangle(const R2Rectangle&);

	/// Force X to yield Expose event with specified
	/// integer rectangle as redraw area.
	/// onExpose() method will be called
    void redrawRectangle(const I2Rectangle&);

	/// Change window title
    void setWindowTitle(const char* title);

	/// Get integer coordinates of point from its real coordinates
    I2Point map(const R2Point& p) const;
    I2Point map(double x, double y) const;

	/// Convert integer x-coordinate to real
    int mapX(double x) const;

	/// Convert interger y-coordinate to real
    int mapY(double y) const;

	/// Get real coordinates of point from its integer coordinates
    R2Point invMap(const I2Point& p) const;

    void recalculateMap();

    // Font methods
    Font loadFont(const char* fontName, XFontStruct **fontStruct = 0);
    void unloadFont(Font fontID);
    XFontStruct* queryFont(Font fontID) const;
    void setFont(Font fontID);

    // Depths supported
    bool supportsDepth24() const;
    bool supportsDepth32() const;
    bool supportsDepth(int d) const;

    // Work with offscreen buffer
    bool createOffscreenBuffer();
    void swapBuffers();

    // Callbacks:
	/// Called when some part of window needs to be redrawed
	virtual void onExpose(XEvent& event);

	/// Called when window has been resized
    virtual void onResize(XEvent& event); // event.xconfigure.width, height

	/// Called when keyboard button was pressed
    virtual void onKeyPress(XEvent& event);

	/// Called when mouse button was pressed (but not released)
    virtual void onButtonPress(XEvent& event);

	/// Called when mouse button was released
    virtual void onButtonRelease(XEvent& event);
    virtual void onMotionNotify(XEvent& event);
    virtual void onCreateNotify(XEvent& event);
    virtual void onDestroyNotify(XEvent& event);
    virtual void onFocusIn(XEvent& event);
    virtual void onFocusOut(XEvent& event);

    // Message from Window Manager, such as "Close Window"
    virtual void onClientMessage(XEvent& event);

    // This method is called from the base implementation of
    // method "onClientMessage". It allows a user application
    // to save all unsaved data and to do other operations before
    // closing a window, when a user pressed the closing box in the upper
    // right corner of a window. The application supplied method should return
    // "true" to close the window or "false" to leave the window open.
    virtual bool onWindowClosing();

    // Message loop
    static bool getNextEvent(XEvent& e);
    static void dispatchEvent(XEvent& e);
    static void messageLoop(GWindow* = 0);

    // For dialog windows
    void doModal();

    // Some methods implementing X-primitives
    virtual void destroyWindow();
    void mapRaised();
    void raise();

private:
    int clip(const R2Point& p1, const R2Point& p2,
                   R2Point& c1,       R2Point& c2);
};

#endif
//
// End of file "gwindow.h"
