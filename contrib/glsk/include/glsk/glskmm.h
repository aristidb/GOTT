
/*
**
**    glskmm - openGL Systems toolKit
**    for C++
**
**    version-info: $Id: glskmm.h 93 2005-02-27 12:51:46Z jax $
**
*/

#ifndef _glskmm_h_
#define _glskmm_h_

#include <string>

#ifndef _glskmm_no_sigc_
#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#endif

/** the glskmm namespace.
*/
namespace glsk
{
    /**
        A convenience type containing two signed integers.
        It is usually used to return coordinates or dimensions.
        It supports and implicit cast to int* and can hence be treated like an int[2].
    */
    struct int2
    {
    private:
        int data[ 2 ];
    public:

        /** Default constructor.
            This constructor leaves the contents of this pair undefined
        */
        inline int2() {}

        /** Initialize from a given value pair.
            \param a Sets the first component to the value specified
            \param b Sets the second component to the value specified
        */
        inline int2( const int a, const int b ) { data[ 0 ] = a; data[ 1 ] = b; }

        /** Cast to int*. */
        inline operator int*() { return data; }
        /** Cast to const int*. */
        inline operator const int*() const { return data; }
        /** Set the contents of this structure. */
        inline void set( const int a, const int b ) { data[ 0 ] = a; data[ 1 ] = b; }
    };

    class pixelformat;
    class mouse_event;
    class window;
    class pbuffer;
    class internal;

    /** An OpenGL rendering context.
        Represents a rendering context that takes OpenGL calls.
        Once this rendering context is selected for a drawable,
        all future OpenGL calls are directed to it.
		\note This is a reference type.
    */
    class context
    {
    private:
        class internal;

        internal*           data;
    public:
        context();
        context( const context& other );

        ~context();

        context&    operator=( const context& other );

        friend class window;
        friend class glsk::internal;
    };
    
    /** A drawable base type.
        This is a type that can be rendered to via OpenGL.  
    */
    #ifndef _glskmm_no_sigc_
    class drawable : public sigc::trackable
    #else
    class drawable
    #endif
    {
        protected:
            inline drawable() {}
        public:
            
            virtual void        swap_buffers() = 0;
            virtual bool        select_rendercontext() = 0;
    };

    /** A window with an opengl client area.
        This class represents window that can be used to draw into.
		\note This is a reference type.
    */    
    class window : public drawable
    {
    public:
        class internal;
    private:
        internal*           data;


    public:
        /*ctor*/                                    window();
        /*copy-ctor*/                               window( const window& other );

        virtual /*dtor*/                            ~window();

        window&                                     operator=( const window& other );
        bool                                        operator==( const window& other ) const;

        bool                                        open( context& rc );
        void                                        destroy();

        void                                        swap_buffers();
        bool                                        select_rendercontext();

        bool                                        show();
        void                                        hide();
        bool                                        is_visible() const;

        // access functions
        const char*                                 get_title() const;
        int                                         get_width() const;
        int                                         get_height() const;
        int2                                        get_size() const;
        int2                                        get_position() const;
        bool                                        get_decorated() const;
        bool                                        get_fullscreen() const;
        bool                                        get_idle_handler() const;

        void                                        set_title( const std::string& string );
        void                                        set_width( const int width );
        void                                        set_height( const int height );
        void                                        set_size( const int width, const int height );
        void                                        set_position( const int x, const int y );
        void                                        set_decorated( const bool value );
        void                                        set_fullscreen( const bool value );
        void                                        set_idle_handler( const bool value );

        virtual void                                on_char( std::string& value );
        virtual void                                on_redraw();
        virtual bool                                on_configure( int x, int y );
        virtual bool                                on_create( int x, int y );
        virtual void                                on_close();
        virtual void                                on_destroy();
        virtual void                                on_pixelformat_select( glsk::pixelformat& format );
        virtual void                                on_idle();
        virtual void                                on_mouse_event( const mouse_event& event );

#ifndef _glskmm_no_sigc_
        sigc::signal< void, std::string& >&         signal_char();
        sigc::signal< void >&                       signal_redraw();
        sigc::signal< bool, int, int >&             signal_configure();
        sigc::signal< bool, int, int >&             signal_create();
        sigc::signal< void >&                       signal_close();
        sigc::signal< void >&                       signal_destroy();
        sigc::signal< void >&                       signal_idle();
        sigc::signal< void,glsk::pixelformat& >&    signal_pixelformat_select();
        sigc::signal< void,glsk::mouse_event& >&    signal_mouse_event();
#endif

        void                                        error_box( const std::string& text ) const;

        friend class glsk::window::internal;
        friend class glsk::internal;

    };
    
    /** Pbuffers are offscreen drawables
		\note This is a reference type.
    */
    class pbuffer : public drawable
    {
    private:
        class internal;
        internal*           data;
    public:
        /*ctor*/            pbuffer();
        /*copy-ctor*/       pbuffer( const pbuffer& other );
        virtual             ~pbuffer();
    
        pbuffer&            operator=( const pbuffer& other );
        bool                operator==( const pbuffer& other ) const;
    
        bool                open( window& owner, context& rc, pixelformat& pixelformat,
                                    int width, int height, bool get_largest );
        void                destroy();
    
        void                swap_buffers();
        bool                select_rendercontext();     
    };

    /** The pixelformat classifies the contents of a drawable.
    */

    class pixelformat
    {
    private:
		class internal;
		internal*		data;

        pixelformat( void* object );

    public:

		pixelformat();
		pixelformat( const pixelformat& other );
		~pixelformat();

		pixelformat& operator=( const pixelformat& other );


        /** The pixelformat flags
            \see set_flags()
            \see get_flags()
        */
        enum flags
        {
            /** Support drawing to a window. */
            draw_to_window = 1,
            /** Support drawing to a pbuffer. */
            draw_to_pbuffer = 2,
            /** Support double buffering. */
            doublebuffer = 4,
            /** Support FSAA via multisampling. \see set_samples() */
            multisample = 8,
            /** Indicate that the color bits are specified. \see set_colorbits() */
            color_bits_set = 16,
            /** Indicate that the depth bits are specified. \see set_depthbits() */
            depth_bits_set = 32,
            /** Indicate that the stencil bits are specified. \see set_stencilbits()  */
            stencil_bits_set = 64
        };

        void    set_flags( const int flags );

        void    set_colorbits( int color_bits );
        void    set_depthbits( int depth_bits );
        void    set_stencilbits( int stencil_bits );
        void    set_samples( int samples );

        int     get_flags() const;
        int     get_colorbits() const;
        int     get_depthbits() const;
        int     get_stencilbits() const;
        int     get_samples() const;

        friend class glsk::window::internal;
        friend class glsk::internal;
    };

    /** Shorthandle for glsk::pixelformat.
    */
    typedef pixelformat pf;

    /** A mouse event structure.
    */

    class mouse_event
    {
    private:
        void*           object;
        mouse_event( void* object );

    public:
        /** The type of this event.
            \see get_type()
        */
        enum type
        {
            /** Mouse move. */
            move,
            /** Mouse button press. */
            button_pressed,
            /** Mouse button release. */
            button_released
        };

        int2            get_position() const;
        type            get_type() const;
        int             get_button() const;

        friend class glsk::window::internal;
    };
    
        
    /** An axial device is an input device that has axial input - typically mice and joysticks.
        \note This is a reference type.
    */
    class axial_device
    {
        private:
            class internal;
            internal*       data;
            
            explicit        axial_device( void* object );
            
        public:
                            axial_device();
                            axial_device( const axial_device& other );
            virtual         ~axial_device();
        
            axial_device&   operator=( const axial_device& other );
            bool            operator==( const axial_device& other ) const;
        
            int             get_button_count() const;
            bool            get_button_state( const int index ) const;
        
            int             get_vector_count() const;
            int2            get_vector_state( const int index ) const;
        
            bool            is_read() const;
            void            mark_read();

            bool            aquire();
            void            unaquire();
            bool            is_aquired() const;

            friend class glsk::internal;        
    };

    /** The program main-loop.
    */
    namespace main
    {
        int             run();
        void            quit();
    };    
    
    /** Keyboard access functions.
    */  
    namespace keyboard
    {
        /** Keycode type.
            \see get_keycode()
        */
        enum keycode
        {
            /** Invalid keycode */
            invalid = 0
        };

        /** Keystate type.
            Reflects the state of a given key at a specific time.
            \see get_keystate()
        */
        enum keystate
        {
            hit = 1,
            held = 2,
            down = 3
        };

        bool            aquire();
        void            unaquire();
        bool            is_aquired();

        keycode         get_keycode( const std::string& name );
        std::string     get_keyname( const keycode code );

        keystate        get_keystate( const keycode code );
    };

    /*

        misc functions

    */

    /** Extension related functions.
    */
    namespace extensions
    {
        int             get_count();
        std::string     get( int index );
        bool            is_supported( const std::string& extensions );
    };


    void                set_resolution( const int width, const int height, const int bpp );
    void                error_box( const std::string& text );      
    axial_device        get_mouse();
    void                print( const std::string& text );

    /** Program entry function.
        To be implemented by the application.
    */
    int                 user_main( int argc, char **argv );

};

#endif
