//-----------------------------------------------------------------------------
///
//      Kształt obiektu definiowany jest w przestrzeni modelu. Obiekty możemy 
//		przemieszczać (translacja), obracać (rotacja), a także zmieniać ich wielkość
//		(skalowanie), używając "transformacji świata" (ang. world transform). 
//		Umieszczamy w ten sposób obiekt w układzie współrzędnych sceny. Następnie
//		musimy w tym układzie ustawić kamerę (oko obserwatora), przez którą będziemy
//		widzieć to, co znajduje się na scenie. Robimy to za pośrednictwem tzw. macierzy
//		widoku. Ostatnim przekształceniem jest tzw. transformacja rzutowania 
//		(ang. projection transform), która umożliwia wykonanie dwuwymiarowego "zdjęcia"
//		naszej trójwymiarowej sceny.
//
//		W niniejszym programie korzystamy z D3DX, zestawu narzędzi pomocniczych dla D3D.
//		Używamy np. funkcji inicjujących macierze transformacji. Aby skorzystać z D3DX
//		należy dołączyć plik nagłówkowy <d3dx9.h> i linkować program z biblioteką d3dx9.lib.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

int global = 0;
bool klik = false;

//-----------------------------------------------------------------------------
// zmienne globalne
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // obiekt Direct3D (obiekt COM)
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // urządzenie wyjściowe (odpowiednik kontekstu graficznego GDI)
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // bufor wierzchołków

int g_nTriangles;


// definicja typu wierzchołka
struct CUSTOMVERTEX
{
  	FLOAT x, y, z;					// współrzędne wierzchołka
	FLOAT norm_x, norm_y, norm_z;	// normalna do powierzchni obiektu wyprowadzona z wierzchołka
};

// definiujemy "flexible vertex format" (FVF) 
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


//-----------------------------------------------------------------------------
// Nazwa: InitD3D()
// Opis: inicjacja Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	// tworzymy obiekt D3D (obiekt COM)
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Ustawiamy strukturę, służącą do utworzenia urządzenia wyjściowego (D3DDevice)
	// Ponieważ tworzymy grafikę 3D, urządzenie musi mieć bufor z.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;		// włączamy bufor z
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // tworzymy urządzenie wyjściowe (D3DDevice)
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // wyłączamy usuwanie niewidocznych powierzchni 
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // włączamy bufor z
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Nazwa: InitGeometry()
// Opis: tworzenie obiektów (zawartości sceny) 
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Tworzymy dwa trójkąty - każdy ma 3 wierzchołki opisane przez 6 liczb. 
    
	CUSTOMVERTEX g_Vertices[] =
    {
		// 1
       	{ -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
        {  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
        {  -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
	
		// 2
		{  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{  1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
        {  -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//3
		{ -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -1.0f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -0.75f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//4
		{ -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -0.75f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -0.75f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//5
		{ 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 1.0f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 0.75f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//6
		{ 1.0f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 0.75f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 0.75f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//7
		{ -0.15f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 0.15f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -0.15f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, },

		//8
		{ 0.15f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ 0.15f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
		{ -0.15f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f, },




		//druga czesc

		// 1
		{ -1.0f, 1.0f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 1.0f, 1.0f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },

		// 2
		{ 1.0f, 1.0f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//3
		{ -1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -1.0f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -0.75f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//4
		{ -1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -0.75f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -0.75f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//5
		{ 1.0f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 1.0f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 0.75f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//6
		{ 1.0f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 0.75f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 0.75f, 0.25f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//7
		{ -0.15f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 0.15f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -0.15f, -2.0f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//8
		{ 0.15f, 0.5f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ 0.15f, -2.0f, -0.25f, 0.0f, 0.0f, -1.0f, },
		{ -0.15f, -2.0f, -0.25f, 0.0f, 0.0f, -1.0f, },

		//boczna sciana 1
		//1
		{ 0.15f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, },
		{ 0.15f, -2.0f, -0.25f, 1.0f, 0.0f, 0.0f, },
		{ 0.15f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f, },
		//2
		{ 0.15f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, },
		{ 0.15f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f, },
		{ 0.15f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, },

		//gorna sciana 2
		//1
		{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
		{ -1.0f, 1.0f, -0.25f, 0.0f, 1.0f, 0.0f, },
		{ 1.0f, 1.0f, -0.25f, 0.0f, 1.0f, 0.0f, },
		//2
		{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
		{ 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
		{ 1.0f, 1.0f, -0.25f, 0.0f, 1.0f, 0.0f, },

		
    };

	// WAŻNE - LICZBA TRÓJKĄTÓW
	g_nTriangles = 20;

    // tworzenie bufora wierchołków
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 3*g_nTriangles*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // kopiujemy dane wierzchołków z tablicy g_Vertices[] do bufora DX
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pVB->Unlock();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Nazwa: Cleanup()
// Opis: zwalnianie zainicjowanych wcześniej obiektów 
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Nazwa: SetupMatrices()
// Opis: ustawienie macierzy świata, widoku i rzutowania 
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	// macierz świata będzie definiować jedynie obrót obiektów w okół osi Y
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

    // Ustawiamy macierz obrotu tak, aby uzyskać 1 pełny obrót (2*PI radianów)
	// co 10 sekund (czyli 10000ms). Aby uniknąć kłopotów związanych z wykonywaniem
	// obliczeń na bardzo dużych liczbach zmiennoprzecinkowych, wyznaczamy resztę 
	// z dzielenia czasu systemowego przez okres obrotu przed dokonaniem konwersji
	// na kąt w radianach.
	D3DXMATRIXA16 matRotation;
    UINT  iTime  = timeGetTime() % 10000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 10000.0f;
    D3DXMatrixRotationY(&matRotation, fAngle );

	D3DXMatrixMultiply(&matWorld, &matRotation, &matWorld);

    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//maciez

	D3DXMATRIXA16 matRotation2;
	UINT  iTime2 = timeGetTime() % 10000;
	FLOAT fAngle2 = iTime * (global * D3DX_PI) / 10000.0f;
	D3DXMatrixRotationX(&matRotation2, fAngle2);

	D3DXMatrixMultiply(&matWorld, &matRotation2, &matWorld);

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Tworzymy macierz widoku. Można ją zdefiniować podając położenie oka,
	// punkt, na który patrzymy, oraz kierunek pionu. Poniżej ustawiamy oko 
	// 4 jednostki od początku układu wzdłuż osi Z i 3 jednostki ponad powierzchnią
	// XZ, patrzymy w początek układu współrzędnych, a pion wskazuje oś Y.
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f, 4.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtRH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );


	// Dla macierzy rzutowania ustawiamy przekształcenie perspektywiczne
	// (dzięki któremu transformacja sceny trójwymiarowej na obraz
	// dwuwymiarowy przebiega z uwzględnieniem perspektywy, a więc obiekty
	// znajdujące się dalej są odwzorowywane jako mniejsze). Aby stworzyć
	// przekształcenie perspektywiczne, musimy podać pole widzenia (1/4 PI
	// to typowa wartość), proporcje osi oraz bliską i daleką płaszczyznę
	// odcinającą (które określają odległości od oka, przy jakich obiekty nie są
	// już renderowane).
	D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovRH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Nazwa: SetupLights()
// Opis: ustawienie oświetlenia sceny
//-----------------------------------------------------------------------------
VOID SetupLights()
{
    // Tworzymy materiał, z którego zbudowany jest obiekt. W danej chwili możemy
	// używać tylko jednego materiału.
	D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 0.2f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 0.8f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.2f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

    // Tworzymy jedno, kierunkowe białe światło
	// Musimy też uruchomić oświetlenie używając stałej D3DRS_LIGHTING w funkcji SetRenderState
	D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    
	vecDir = D3DXVECTOR3(0.0f,
                         -0.2f,
                         -1.0f);

    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;

	g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
   
}




//-----------------------------------------------------------------------------
// Nazwa: Render()
// Opis: rysowanie sceny
//-----------------------------------------------------------------------------
VOID Render()
{
    // ustawiamy kolor tła
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(0,70,70), 1.0f, 0 );

    // rozpoczęcie rysowania sceny
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		// ustawienie świateł i materiałów 
        SetupLights();

        // ustawienie macierzy świata, widoku i rzutowania 
        SetupMatrices();

		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

        // renderowanie zawartości bufora wierzchołków
        g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, g_nTriangles );

		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

        // zakończenie rysowania sceny
        g_pd3dDevice->EndScene();
    }

    // wyświetlenie sceny na ekranie
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Nazwa: MsgProc()
// Opis: obsługa komunikatów wysyłanych do naszego okna
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static int x, y;
	static int startx, starty;
	
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;

		case WM_LBUTTONDOWN: //reakcja na lewy przycisk myszki
		{
			klik = true;
			startx = LOWORD(lParam);
			starty = HIWORD(lParam);
		
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			if (klik == true)
			{
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				global = (y-starty)*(x-startx);
			}
		}

		case WM_LBUTTONUP: //reakcja na lewy przycisk myszki
		{
			klik = false;
		}

    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Nazwa: WinMain()
// Opis: program główny
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // rejestracja klasy okna 
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "GK D3D", NULL };
    RegisterClassEx( &wc );

    // utworzenie okna naszej aplikacji 
	HWND hWnd = CreateWindow( "GK D3D", "Grafika Komputerowa: DirectX",
                              WS_OVERLAPPEDWINDOW, 100, 100, 512, 512,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // inicjacja Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // utworzenie obiektów sceny
        if( SUCCEEDED( InitGeometry() ) )
        {
            // wyświetlenie okna
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // uruchomienie pętli obsługi komunikatów
            MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();	// <-- RYSOWANIE SCENY
            }
        }
    }

    UnregisterClass( "GK D3D", wc.hInstance );
    return 0;
}

