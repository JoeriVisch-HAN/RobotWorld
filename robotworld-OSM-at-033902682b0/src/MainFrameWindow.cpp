#include "MainFrameWindow.hpp"

#include "Button.hpp"
#include "Checkbox.hpp"
#include "Client.hpp"
#include "FileTraceFunction.hpp"
#include "Logger.hpp"
#include "LogTextCtrl.hpp"
#include "MainApplication.hpp"
#include "MathUtils.hpp"
#include "Message.hpp"
#include "MessageTypes.hpp"
#include "Radiobox.hpp"
#include "Robot.hpp"
#include "RobotShape.hpp"
#include "RobotWorld.hpp"
#include "RobotWorldCanvas.hpp"
#include "Shape2DUtils.hpp"
#include "StdOutTraceFunction.hpp"
#include "Trace.hpp"
#include "WidgetTraceFunction.hpp"

#include <array>
#include <iostream>

/**
 * This handler calls the supplied lambda function when the handleResponse function is called
 */
template< typename T >
struct CallbackResponseHandler : public Messaging::ResponseHandler
{
	explicit CallbackResponseHandler(T aT) : t(aT){}
	/**
	 * @see Messaging::ResponseHandler::handleResponse( const Message& aMessage)
	 */
	virtual void handleResponse( const Messaging::Message& aMessage)
	{
		FUNCTRACE_TEXT_DEVELOP(aMessage.asString());
		t(aMessage);
	}
	T t;
}; // struct CallbackResponseHandler

namespace Application
{
	/**
	 * IDs for the controls and the menu commands
	 * If there are (default) wxWidget ID's: try to maintain
	 * compatibility, especially wxID_ABOUT because on a Mac it is special
	 */
	enum
	{
		ID_QUIT 	= wxID_EXIT,         	//!< ID_QUIT
		ID_OPTIONS 	= wxID_PROPERTIES,		//!< ID_OPTIONS
		ID_ABOUT 	= wxID_ABOUT,        	//!< ID_ABOUT
		ID_WIDGET_TRACE_FUNCTION, 			//!< ID_WIDGET_TRACE_FUNCTION
		ID_STDCOUT_TRACE_FUNCTION, 			//!< ID_STDCOUT_TRACE_FUNCTION
		ID_FILE_TRACE_FUNCTION 				//!< ID_FILE_TRACE_FUNCTION
	};
	/**
	 *
	 */
	MainFrameWindow::MainFrameWindow( const std::string& aTitle) :
																wxFrame( nullptr, wxID_ANY, aTitle, wxDefaultPosition, wxSize( 1200, 600)),
																clientPanel( nullptr),
																menuBar( nullptr),
																splitterWindow( nullptr),
																lhsPanel( nullptr),
																robotWorldCanvas( nullptr),
																rhsPanel( nullptr),
																logTextCtrl( nullptr),
																logDestination( nullptr),
																configPanel(nullptr),
																speedSpinCtrl(nullptr),
																worldNumber(nullptr),
																buttonPanel( nullptr)
	{
		initialise();
	}
	/**
	 *
	 */
	void MainFrameWindow::initialise()
	{
		SetMenuBar( initialiseMenuBar());

		wxGridBagSizer* sizer = new wxGridBagSizer( 5, 5);

		sizer->Add( initialiseClientPanel(),
					wxGBPosition( 0, 0), 	// row ,col
					wxGBSpan( 1, 1), 		// row ,col
					wxGROW);
		sizer->AddGrowableCol( 0);
		sizer->AddGrowableRow( 0);

		SetSizer( sizer);
		sizer->SetSizeHints( this);

		Bind( wxEVT_COMMAND_MENU_SELECTED,
			  [this](wxCommandEvent& anEvent){ this->OnQuit(anEvent);},
			  ID_QUIT);
		Bind( wxEVT_COMMAND_MENU_SELECTED,
			  [this](wxCommandEvent& anEvent){ this->OnWidgetTraceFunction(anEvent);},
			  ID_WIDGET_TRACE_FUNCTION);
		Bind( wxEVT_COMMAND_MENU_SELECTED,
			  [this](wxCommandEvent& anEvent){ this->OnStdOutTraceFunction(anEvent);},
			  ID_STDCOUT_TRACE_FUNCTION);
		Bind( wxEVT_COMMAND_MENU_SELECTED,
			  [this](wxCommandEvent& anEvent){ this->OnFileTraceFunction(anEvent);},
			  ID_FILE_TRACE_FUNCTION);
		Bind( wxEVT_COMMAND_MENU_SELECTED,
			  [this](wxCommandEvent& anEvent){ this->OnAbout(anEvent);},
			  ID_ABOUT);

		// By default we use the WidgettraceFunction as we expect that this is what the user wants....
		Base::Trace::setTraceFunction( std::make_unique<Application::WidgetTraceFunction>(logTextCtrl));
	}
	/**
	 *
	 */
	wxMenuBar* MainFrameWindow::initialiseMenuBar()
	{
		wxMenu* fileMenu = new wxMenu;
		fileMenu->Append( ID_QUIT, "E&xit\tAlt-X", "Exit the application");

		wxMenu* debugMenu = new wxMenu;
		debugMenu->AppendRadioItem( ID_WIDGET_TRACE_FUNCTION,  "Widget",  "Widget");
		debugMenu->AppendRadioItem( ID_STDCOUT_TRACE_FUNCTION,  "StdOut",  "StdOut");
		debugMenu->AppendRadioItem( ID_FILE_TRACE_FUNCTION,  "File",  "File");

		wxMenu* helpMenu = new wxMenu;
		helpMenu->Append( ID_ABOUT, "&About...\tF1", "Show about dialog");

		menuBar = new wxMenuBar;
		menuBar->Append( fileMenu, "&File");
		menuBar->Append( debugMenu, "&Debug");
		menuBar->Append( helpMenu, "&Help");

		return menuBar;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseClientPanel()
	{
		clientPanel = new wxPanel( this);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0));

		sizer->Add( initialiseSplitterWindow(),
					wxGBPosition( 1, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 1);
		sizer->AddGrowableCol( 1);

		sizer->Add( 5, 5,
					wxGBPosition( 2, 2));

		clientPanel->SetSizer( sizer);
		sizer->SetSizeHints( clientPanel);

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(clientPanel,sizer);
		}

		return clientPanel;
	}
	/**
	 *
	 */
	wxSplitterWindow* MainFrameWindow::initialiseSplitterWindow()
	{
		splitterWindow = new wxSplitterWindow( clientPanel);
		splitterWindow->SetSashInvisible();
		splitterWindow->SplitVertically( initialiseLhsPanel(), initialiseRhsPanel());
		splitterWindow->SetSashPosition(lhsPanel->GetSize().GetWidth());
		return splitterWindow;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseLhsPanel()
	{
		lhsPanel = new wxPanel( splitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 0);

		sizer->Add( robotWorldCanvas = new View::RobotWorldCanvas( lhsPanel),
					wxGBPosition( 1, 1),
					wxGBSpan( 1, 1),
					wxSHRINK);
		robotWorldCanvas->SetMinSize( wxSize( 500,500));

		sizer->Add( 5, 5,
					wxGBPosition( 2, 2),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 2);

		lhsPanel->SetSizer( sizer);
		sizer->SetSizeHints( lhsPanel);

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(lhsPanel,sizer);
		}

		return lhsPanel;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseRhsPanel()
	{
		rhsPanel = new wxPanel( splitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 0);

		sizer->Add( logPanel = initialiseLogPanel(),
					wxGBPosition( 2, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( configPanel = initialiseConfigPanel(),
					wxGBPosition( 4, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( buttonPanel = initialiseButtonPanel(),
					wxGBPosition( 6, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableCol( 1);

		sizer->Add( 5, 5,
					wxGBPosition( 7, 2),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 7);


		rhsPanel->SetSizer( sizer);
		sizer->SetSizeHints( rhsPanel);

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(rhsPanel,sizer);
		}

		return rhsPanel;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseLogPanel()
	{
		wxPanel* panel = new wxPanel( rhsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0),
					wxGBSpan( 1, 1),
					wxSHRINK);

		std::array<std::string,3> choicesArray
		{
			"Window",
			"StdOut",
			"File"
		};

		sizer->Add(	logDestination = makeRadiobox(	panel,
													choicesArray,
													[this](wxCommandEvent& event)
													{
														wxRadioBox* radiobox = dynamic_cast< wxRadioBox* >(event.GetEventObject());
														if(radiobox)
														{
															switch(radiobox->GetSelection())
															{
																case 0:
																{
																	OnWidgetTraceFunction(event);
																	break;
																}
																case 1:
																{
																	OnStdOutTraceFunction(event);
																	break;
																}
																case 2:
																{
																	OnFileTraceFunction(event);
																	break;
																}
																default:
																{
																	TRACE_DEVELOP("Unknown trace destination");
																}
															}
														}
													},
													"Log destination",
													wxRA_SPECIFY_COLS),
					wxGBPosition( 1, 1),
					wxGBSpan( 1, 1),
					wxALIGN_CENTER);

		sizer->Add( logTextCtrl = new LogTextCtrl( panel, wxID_ANY, wxTE_MULTILINE | wxTE_DONTWRAP),
					wxGBPosition( 2, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableRow( 2);
		sizer->AddGrowableCol( 1);
		logTextCtrl->SetMinSize( wxSize( 500, 250));

		sizer->Add( makeButton( panel,
								"Clear log window",
								[this](wxCommandEvent& /*anEvent*/){logTextCtrl->Clear();}),
					wxGBPosition( 3, 1),
					wxGBSpan( 1, 1),
					wxGROW |wxALIGN_CENTER);

		sizer->Add( 5, 5,
					wxGBPosition( 4, 2),
					wxGBSpan( 1, 1),
					wxSHRINK);

		panel->SetSizerAndFit( sizer);

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(panel,sizer);
		}

		return panel;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseConfigPanel()
	{
		wxPanel* panel = new wxPanel( rhsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableCol( 0);



		/////// Speed
		sizer->Add(new wxStaticText(panel,
									wxID_ANY,
									"Speed"),
				   wxGBPosition( 2, 1),
				   wxGBSpan( 1, 1),
				   wxSHRINK | wxALIGN_CENTER);
		sizer->Add(speedSpinCtrl = new wxSpinCtrl(panel,
												  wxID_ANY),
				   wxGBPosition( 2, 2),
				   wxGBSpan( 1, 1),
				   wxSHRINK | wxALIGN_CENTER);
		speedSpinCtrl->SetValue(static_cast<int>(10));
		speedSpinCtrl->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED,[this](wxCommandEvent& event){this->OnSpeedSpinCtrlUpdate(event);});

		std::array<std::string,6> choicesArray
		{
			"Situatie 1 A",
			"Situatie 1 B",
			"Situatie 2 A",
			"Situatie 2 B",
			"Situatie 3 A",
			"Situatie 3 B"

		};

		sizer->Add(	worldNumber = makeRadiobox(	panel,
												choicesArray,
												[this](wxCommandEvent& event)
												{
													wxRadioBox* radiobox = dynamic_cast< wxRadioBox* >(event.GetEventObject());
													if(radiobox)
													{
														switch(radiobox->GetSelection())
														{
															case 0:
															{
																OnWorld1(event);
																break;
															}
															case 1:
															{
																OnWorld2(event);
																break;
															}
															case 2:
															{
																OnWorld3(event);
																break;
															}
															case 3:
															{
																OnWorld4(event);
																break;
															}
															case 4:
															{
																OnWorld5(event);
																break;
															}
															case 5:
															{
																OnWorld6(event);
																break;
															}

															default:
															{
																TRACE_DEVELOP("Unknown world selection");
															}
														}
													}
												},
												"World number",
												wxRA_SPECIFY_ROWS),
					wxGBPosition( 3, 1),
					wxGBSpan( 1, 1),
					wxSHRINK | wxALIGN_CENTER);
		sizer->AddGrowableRow( 3);
		sizer->AddGrowableCol( 1);

		sizer->Add( 5, 5,
					wxGBPosition( 4, 3),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableCol( 3);

		panel->SetSizerAndFit( sizer);

		MainSettings& mainSettings = MainApplication::getSettings();
		speedSpinCtrl->SetValue(static_cast<int>(mainSettings.getSpeed()));
		worldNumber->SetSelection(static_cast<int>(mainSettings.getWorldNumber()));

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(panel,sizer);
		}

		return panel;
	}
	/**
	 *
	 */
	wxPanel* MainFrameWindow::initialiseButtonPanel()
	{
		wxPanel* panel = new wxPanel( rhsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER);

		wxGridBagSizer* sizer = new wxGridBagSizer();

		sizer->Add( 5, 5,
					wxGBPosition( 0, 0),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableCol( 0);

		sizer->Add( makeButton( panel,
								"Populate",
								[this](wxCommandEvent& anEvent){this->OnPopulate(anEvent);}),
					wxGBPosition( 1, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( makeButton( panel,
								"Unpopulate",
								[this](wxCommandEvent& anEvent){this->OnUnpopulate(anEvent);}),
					wxGBPosition( 1, 2),
					wxGBSpan( 1, 1),
					wxGROW);

		sizer->Add( makeButton( panel,
								"Start robot",
								[this](wxCommandEvent& anEvent){this->OnStartRobot(anEvent);}),
					wxGBPosition( 3, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( makeButton( panel,
								"Stop robot",
								[this](wxCommandEvent& anEvent){this->OnStopRobot(anEvent);}),
					wxGBPosition( 3, 2),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( makeButton( panel,
								"Start listening",
								[this](wxCommandEvent& anEvent){this->OnStartListening(anEvent);}),
					wxGBPosition( 5, 1),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( makeButton( panel,
								"Send message",
								[this](wxCommandEvent& anEvent){this->OnSendMessage(anEvent);}),
					wxGBPosition( 5, 2),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->Add( makeButton( panel,
								"Stop listening",
								[this](wxCommandEvent& anEvent){this->OnStopListening(anEvent);}),
					wxGBPosition( 5, 3),
					wxGBSpan( 1, 1),
					wxGROW);

		sizer->Add( 5, 5,
					wxGBPosition( 6, 4),
					wxGBSpan( 1, 1),
					wxGROW);
		sizer->AddGrowableCol( 4);

		panel->SetSizerAndFit( sizer);

		if(MainApplication::isArgGiven("-debug_grid"))
		{
			showGridFor(panel,sizer);
		}

		return panel;
	}


	/**
	 *
	 */
	void MainFrameWindow::OnQuit( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Base::Trace::setTraceFunction( std::make_unique<Base::StdOutTraceFunction>());
		Close( true);
	}
	/**
	 *
	 */
	void MainFrameWindow::OnWidgetTraceFunction( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Base::Trace::setTraceFunction( std::make_unique<Application::WidgetTraceFunction>(logTextCtrl));

		wxMenuItem* item = menuBar->FindItem(ID_WIDGET_TRACE_FUNCTION);
		if(item && item->IsRadio() && !item->IsCheck())
		{
			item->Check();
		}
		logDestination->SetSelection(0);
	}
	/**
	 *
	 */
	void MainFrameWindow::OnStdOutTraceFunction( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Base::Trace::setTraceFunction( std::make_unique<Base::StdOutTraceFunction>());

		wxMenuItem* item = menuBar->FindItem(ID_STDCOUT_TRACE_FUNCTION);
		if(item && item->IsRadio() && !item->IsCheck())
		{
			item->Check();
		}
		logDestination->SetSelection(1);
	}
	/**
	 *
	 */
	void MainFrameWindow::OnFileTraceFunction( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Base::Trace::setTraceFunction( std::make_unique<Base::FileTraceFunction>("trace", "log", true));

		wxMenuItem* item = menuBar->FindItem(ID_FILE_TRACE_FUNCTION);
		if(item && item->IsRadio() && !item->IsCheck())
		{
			item->Check();
		}
		logDestination->SetSelection(2);
	}
	/**
	 *
	 */
	void MainFrameWindow::OnAbout( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		wxMessageBox( "ESD 2012-present RobotWorld.\n", "About RobotWorld", wxOK | wxICON_INFORMATION, this);
	}

	/**
	 *
	 */
	void MainFrameWindow::OnSpeedSpinCtrlUpdate( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
//		TRACE_DEVELOP(anEvent.GetString().ToStdString());
//		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getRobot( "Robot");
//		if (robot)
//		{
//			robot->setSpeed(static_cast<float>(speedSpinCtrl->GetValue()));
//		}

		MainSettings& mainSettings = MainApplication::getSettings();
		mainSettings.setSpeed(speedSpinCtrl->GetValue());
	}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld1( wxCommandEvent& anEvent)
	{
		TRACE_DEVELOP(anEvent.GetString().ToStdString());

		MainSettings& mainSettings = MainApplication::getSettings();
		mainSettings.setWorldNumber(worldNumber->GetSelection());
	}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld2( wxCommandEvent& anEvent)
	{
		TRACE_DEVELOP(anEvent.GetString().ToStdString());

		MainSettings& mainSettings = MainApplication::getSettings();
		mainSettings.setWorldNumber(worldNumber->GetSelection());
	}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld3( wxCommandEvent& anEvent)
	{
		TRACE_DEVELOP(anEvent.GetString().ToStdString());

		MainSettings& mainSettings = MainApplication::getSettings();
		mainSettings.setWorldNumber(worldNumber->GetSelection());
	}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld4( wxCommandEvent& anEvent)
		{
			TRACE_DEVELOP(anEvent.GetString().ToStdString());

			MainSettings& mainSettings = MainApplication::getSettings();
			mainSettings.setWorldNumber(worldNumber->GetSelection());
		}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld5( wxCommandEvent& anEvent)
		{
			TRACE_DEVELOP(anEvent.GetString().ToStdString());

			MainSettings& mainSettings = MainApplication::getSettings();
			mainSettings.setWorldNumber(worldNumber->GetSelection());
		}
	/**
	 *
	 */
	void MainFrameWindow::OnWorld6( wxCommandEvent& anEvent)
		{
			TRACE_DEVELOP(anEvent.GetString().ToStdString());

			MainSettings& mainSettings = MainApplication::getSettings();
			mainSettings.setWorldNumber(worldNumber->GetSelection());
		}
	/**
	 *
	 */
	void MainFrameWindow::OnStartRobot( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		for (Model::RobotPtr robot : Model::RobotWorld::getRobotWorld().getRobots())
		{
			if (!robot->isActing())
			{
				robot->startActing();
			}
		}
		TRACE_DEVELOP("Started Robots");
		Model::RobotPtr localRobot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		localRobot->sendMessage( Messaging::Message( Messaging::StartRequest ));
	}
	/**
	 *
	 */
	void MainFrameWindow::OnStopRobot( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		if (robot && robot->isActing())
		{
			robot->stopActing();
			TRACE_DEVELOP("Stopped Local Robot");
			robot->sendMessage( Messaging::Message( Messaging::StopRequest ));
		}
	}
	/**
	 *
	 */

	/**
	 * This is the callback function that is used by the CallbackResponseHandler
	 */
	void HandleResponse( const Messaging::Message& aMessage)
	{
		FUNCTRACE_TEXT_DEVELOP(aMessage.asString());
	}

	void MainFrameWindow::OnPopulate( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		int worldSelection = worldNumber->GetSelection();
		robotWorldCanvas->populate(worldSelection, false);
		TRACE_DEVELOP("Spawned world " + worldSelection);
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		robot->sendMessage( Messaging::Message( Messaging::PopulateRequest, std::to_string(worldSelection)) );
	}
	/**
	 *
	 */
	void MainFrameWindow::OnUnpopulate( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		robotWorldCanvas->unpopulate();

		logTextCtrl->Clear();
	}
	/**
	 *
	 */
	void MainFrameWindow::OnStartListening( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		if (robot)
		{
			robot->startCommunicating();
		} else
		{
			TRACE_DEVELOP("No robot found! Unable to start server.");
		}
	}
	/**
	 *
	 */
	void MainFrameWindow::OnSendMessage( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		robot->sendMessage( Messaging::Message( Messaging::EchoRequest, "Hello world!") );
	}
	/**
	 *
	 */
	void MainFrameWindow::OnStopListening( wxCommandEvent& UNUSEDPARAM(anEvent))
	{
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		if (robot)
		{
			robot->stopCommunicating();
		}
	}

	void MainFrameWindow::sendMessage(const Messaging::Message& message)
	{
		Model::RobotPtr robot = Model::RobotWorld::getRobotWorld().getLocalRobot();
		robot->sendMessage( message );
	}

	/**
	 *
	 */
	void MainFrameWindow::showGridFor(wxPanel* aPanel, wxGridBagSizer* aSizer)
	{
		aPanel->Bind(wxEVT_PAINT,[aPanel,aSizer](wxPaintEvent& /*event*/)
		{
			wxPaintDC dc( aPanel );

			wxGridBagSizer* gridBagSizer = aSizer;
			if(gridBagSizer)
			{
				wxSize screenSize = aPanel->GetSize();

				dc.SetPen( wxPen("BLACK", 1, wxPENSTYLE_SOLID));

				int rowY = 0;
				for(int row = 0; row < gridBagSizer->GetRows(); ++row)
				{
					wxSize cellSize = gridBagSizer->GetCellSize(row,0);
					rowY += cellSize.y;
					dc.DrawLine(0, rowY, screenSize.x, rowY);
				}
				int colX = 0;
				for(int col = 0; col < gridBagSizer->GetCols(); ++col)
				{
					wxSize cellSize = gridBagSizer->GetCellSize(0,col);
					colX += cellSize.x;
					dc.DrawLine(colX, 0, colX, screenSize.y);
				}
			}
		});
		aPanel->Bind(wxEVT_SIZE,[aPanel,aSizer](wxSizeEvent& /*event*/)
		{
			aPanel->Layout();
			aPanel->Refresh();
		});
	}
} // namespace Application
