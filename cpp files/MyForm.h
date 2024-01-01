#pragma once

#include"API_Interaction.h"
#include"AI_Bot.h"
#include <msclr/marshal_cppstd.h>
#include<thread>



namespace $safeprojectname$ {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	/// 
	
	public ref class MyForm : public System::Windows::Forms::Form

	{
	private: System::Threading::Thread^ th1;                                //Creating a thread for the background tasks like for simulating realtime fetching of data

		   ////////////////////////////////////////////////////////////////
	private: System::Windows::Forms::CheckBox^ aiCheckBox;
	private: System::Windows::Forms::ListBox^ aiTimeFrameListBox;
	private: System::Windows::Forms::TextBox^ onScreenLogs;
	private: System::Windows::Forms::Button^ ReloadButton;
	private: System::Windows::Forms::Label^ availableBalanceHeading;
	private: System::Windows::Forms::Label^ orderBookHeading;
	private: System::Windows::Forms::Label^ priceHeading;
	private: System::Windows::Forms::Label^ marginHeading;
	private: System::Windows::Forms::Label^ orderTypeHeading;
	private: System::Windows::Forms::Label^ symbolsHeading;
	private: System::Windows::Forms::Label^ leverageHeading;
	private: System::Windows::Forms::Label^ aiBotHeading;
	private: System::Windows::Forms::Label^ MarkPriceHeading;
	private: System::Windows::Forms::Label^ openPositionsHeading;
	private: System::Windows::Forms::Label^ openOrdersHeading;
	private: System::Windows::Forms::Label^ aiTradeStatusHeading;
	private: System::Windows::Forms::Label^ aiTimeFrameHeading;
	private: System::Windows::Forms::Label^ positionSideValue;
	private: System::Windows::Forms::Label^ positionAmountHeading;
	private: System::Windows::Forms::Label^ positionLeverageHeading;
	private: System::Windows::Forms::Label^ positionPnlHeading;
	private: System::Windows::Forms::Label^ orderQuantityHeading;
	private: System::Windows::Forms::Label^ orderSideHeading;
	private: System::Windows::Forms::Label^ orderSymbolHeading;
	private: System::Windows::Forms::Label^ orderPriceHeading;
	private: System::Windows::Forms::Label^ positionQuantityHeading;
	private: System::Windows::Forms::Label^ positionSideHeading;
	private: System::Windows::Forms::Label^ positionSymbolHeading;
	private: System::Windows::Forms::Label^ tradeStatusBarHeading;
	private: System::Windows::Forms::Label^ marginValue;
	private: System::Windows::Forms::Label^ symbolSelectionMsg;
	private: System::Windows::Forms::Label^ orderTypeSelectionMsg;
	private: System::Windows::Forms::Label^ positionEntryPriceHeading;
	private: System::Windows::Forms::Button^ openChartButton;
		   ////////////////////////////////////////////////////////////////

	private: int previousMarkPrice = -1;          //To store the previous mark price for comparison

		   delegate void UpdateUIDef();          //Delegate for updating the UI
		   delegate void TheOtherOne();           //Delegate for updating the UI
	public:
		MyForm(void)
		{
			InitializeComponent();         //Auto Generated Code by Windows Form Application

			th1 = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &MyForm::updateThingsInRealTimeThreadFunction));  //Creating a thread for the background tasks like for simulating realtime fetching of data
			th1->Start();          //Starting the thread
		}
		void updateThingsInRealTimeThreadFunction()
		{
			std::chrono::milliseconds timespan(15000);        //This thread will sleep for 15 seconds and then update the UI

			while (true)
			{

				std::this_thread::sleep_for(timespan);           //Sleeping for 15 seconds

				//Winforms application has a prebuilt thread by default. And it is not possible to update the UI from a thread other than the prebuilt thread. 
				// So we need to invoke the prebuilt thread to update the UI from the background thread. But because the UI thread will become busy doing the work that we will assign it now
				// it will not be able to update the UI. So the UI will become stuck for that amount of time. The time is really noticeable but we cannot do anything about it because it
				//	is a limitation of the Winforms application and the network requests take that much time as well. 
				if (this->InvokeRequired) {                       
					onScreenLogs->BeginInvoke(gcnew UpdateUIDef(this, &MyForm::UpdateOrderBookOpenPositionsAndOrders));
					MarkPriceValue->BeginInvoke(gcnew TheOtherOne(this, &MyForm::UpdateMarkPrice));

				}
			}
		}


		void UpdateMarkPrice() {           //This function is only concerned with updating the Status texts and the Mark Price
			aiTradeStatusBar->Text = "";
			aiTradeStatusBar->ForeColor=System::Drawing::Color::Black;
			TradeStatusBar->Text = "";
			TradeStatusBar->ForeColor = System::Drawing::Color::Black;
			if (symbolsListBox->SelectedIndex != -1) {
				std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());
				if(symbol != "Try Again After Reloading. ")
				{
					std::string result = fetchRealTimeValue(symbol);
					if (result == "Try Again After Reloading. ") {
						onScreenLogs->Text += "Please Reload and select a valid symbol. \n";
					}
					else {
						double markPrice = std::stod(result);         
						result.pop_back();
						result.pop_back();            //Had to use this approach to remove the trailing zeros
						result.pop_back();
						result.pop_back();
						result.pop_back();
						result.pop_back();
						if(previousMarkPrice==-1)
							previousMarkPrice = markPrice;
						else if(previousMarkPrice>markPrice)
							MarkPriceValue->ForeColor = System::Drawing::Color::Red;
						else if(previousMarkPrice<markPrice)
							MarkPriceValue->ForeColor = System::Drawing::Color::Green;
						MarkPriceValue->Text = gcnew String(result.c_str());
						onScreenLogs->Text += "Market Price Updated. \n";
					}
				}
				else
					onScreenLogs->Text+="Please Reload and select a valid symbol. \n";
			}
		}
		void UpdateOrderBookOpenPositionsAndOrders() {      //This function is only concerned with updating the Order Book
			if (symbolsListBox->SelectedIndex != -1) {
				std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());
				if (symbol != "Try Reloading.") {
					onScreenLogs->Text += "Order Book Updated. \n";
					try {
						std::vector<std::string> orderBook = fetchOrderBook(symbol);      //Fetching the order book
						if (orderBook[0] == "Try Reloading.") {
							onScreenLogs->Text += "Couldn't update the Order Book. \n";
						}
						else {
							orderBookValue1->Text = gcnew String(orderBook[0].c_str());
							orderBookValue2->Text = gcnew String(orderBook[1].c_str());
							orderBookValue3->Text = gcnew String(orderBook[2].c_str());
							orderBookValue4->Text = gcnew String(orderBook[3].c_str());
							orderBookValue5->Text = gcnew String(orderBook[4].c_str());          //Updating the UI
							orderBookValue6->Text = gcnew String(orderBook[5].c_str());
							orderBookValue7->Text = gcnew String(orderBook[6].c_str());
							orderBookValue8->Text = gcnew String(orderBook[7].c_str());
							orderBookValue9->Text = gcnew String(orderBook[8].c_str());
							orderBookValue10->Text = gcnew String(orderBook[9].c_str());
						}
					}
					catch (const std::exception& e) {
						onScreenLogs->Text+="Couldn't update the Order Book. \n";
						std::string error = e.what();
						onScreenLogs->Text += gcnew String(error.c_str());
					}
				}
				else {
					onScreenLogs->Text += "Please Reload and select a valid symbol. \n";
				}

			}
			positionsInitializer(positionsStartIndex);               //Fetching and updating the Open Positions
			ordersInitializer(ordersStartIndex);         //Fetching and updating the Open Orders
			
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)                     //Auto Generated Code by Windows Form Application
			{
				delete components;
			}
		}
		 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// All the code below this is auto generated by Windows Form Application
	private: System::Windows::Forms::ListBox^ symbolsListBox;
	private: System::Windows::Forms::Label^ leverageValue;
	private: System::Windows::Forms::TrackBar^ leverageSlider;
	private: System::Windows::Forms::Label^ availableBalanceValue;
	private: System::Windows::Forms::ListBox^ OrderTypeListBox;
	private: System::Windows::Forms::TextBox^ priceTextBox;
	private: System::Windows::Forms::TrackBar^ marginSlider;
	private: System::Windows::Forms::Button^ buyButton;
	private: System::Windows::Forms::Button^ sellButton;
	private: System::Windows::Forms::Label^ orderBookValue1;
	private: System::Windows::Forms::Label^ orderBookValue2;
	private: System::Windows::Forms::Label^ orderBookValue3;
	private: System::Windows::Forms::Label^ orderBookValue4;
	private: System::Windows::Forms::Label^ orderBookValue5;
	private: System::Windows::Forms::Label^ orderBookValue6;
	private: System::Windows::Forms::Label^ MarkPriceValue;
	private: System::Windows::Forms::Label^ orderBookValue7;
	private: System::Windows::Forms::Label^ orderBookValue8;
	private: System::Windows::Forms::Label^ orderBookValue9;
	private: System::Windows::Forms::Label^ orderBookValue10;
	private: System::Windows::Forms::Label^ positionAmountValue;
	private: System::Windows::Forms::Label^ positionEntryPriceValue;
	private: System::Windows::Forms::Label^ positionLeverageValue;
	private: System::Windows::Forms::Label^ positionPnlValue;
	private: System::Windows::Forms::Label^ positionQuantityValue;
	private: System::Windows::Forms::Label^ positionSymbolValue;
	private: System::Windows::Forms::Button^ Next_Position;
	private: System::Windows::Forms::Label^ orderSymbolValue;
	private: System::Windows::Forms::Label^ orderPriceValue;
	private: System::Windows::Forms::Label^ orderQuantityValue;
	private: System::Windows::Forms::Label^ orderSideValue;
	private: System::Windows::Forms::Button^ nextOrderButton;
	private: System::Windows::Forms::Label^ aiValueStorage1;
	private: System::Windows::Forms::Label^ TradeStatusBar;
	private: System::Windows::Forms::Label^ aiValueStorage2;
	private: System::Windows::Forms::Label^ aiTradeStatusBar;
	private: System::Windows::Forms::Label^ aiValueStorage3;
	private: System::Windows::Forms::Label^ aiValueStorage4;
	private: System::Windows::Forms::Button^ cancelOrderButton;
	private: System::Windows::Forms::Button^ closePositionButton;




	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->symbolsListBox = (gcnew System::Windows::Forms::ListBox());
			this->leverageValue = (gcnew System::Windows::Forms::Label());
			this->leverageSlider = (gcnew System::Windows::Forms::TrackBar());
			this->availableBalanceValue = (gcnew System::Windows::Forms::Label());
			this->OrderTypeListBox = (gcnew System::Windows::Forms::ListBox());
			this->priceTextBox = (gcnew System::Windows::Forms::TextBox());
			this->marginSlider = (gcnew System::Windows::Forms::TrackBar());
			this->buyButton = (gcnew System::Windows::Forms::Button());
			this->sellButton = (gcnew System::Windows::Forms::Button());
			this->orderBookValue1 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue2 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue3 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue4 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue5 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue6 = (gcnew System::Windows::Forms::Label());
			this->MarkPriceValue = (gcnew System::Windows::Forms::Label());
			this->orderBookValue7 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue8 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue9 = (gcnew System::Windows::Forms::Label());
			this->orderBookValue10 = (gcnew System::Windows::Forms::Label());
			this->positionAmountValue = (gcnew System::Windows::Forms::Label());
			this->positionEntryPriceValue = (gcnew System::Windows::Forms::Label());
			this->positionLeverageValue = (gcnew System::Windows::Forms::Label());
			this->positionPnlValue = (gcnew System::Windows::Forms::Label());
			this->positionQuantityValue = (gcnew System::Windows::Forms::Label());
			this->positionSymbolValue = (gcnew System::Windows::Forms::Label());
			this->Next_Position = (gcnew System::Windows::Forms::Button());
			this->orderSymbolValue = (gcnew System::Windows::Forms::Label());
			this->orderPriceValue = (gcnew System::Windows::Forms::Label());
			this->orderQuantityValue = (gcnew System::Windows::Forms::Label());
			this->orderSideValue = (gcnew System::Windows::Forms::Label());
			this->nextOrderButton = (gcnew System::Windows::Forms::Button());
			this->aiValueStorage1 = (gcnew System::Windows::Forms::Label());
			this->TradeStatusBar = (gcnew System::Windows::Forms::Label());
			this->aiValueStorage2 = (gcnew System::Windows::Forms::Label());
			this->aiTradeStatusBar = (gcnew System::Windows::Forms::Label());
			this->aiValueStorage3 = (gcnew System::Windows::Forms::Label());
			this->aiValueStorage4 = (gcnew System::Windows::Forms::Label());
			this->cancelOrderButton = (gcnew System::Windows::Forms::Button());
			this->closePositionButton = (gcnew System::Windows::Forms::Button());
			this->onScreenLogs = (gcnew System::Windows::Forms::TextBox());
			this->aiCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->aiTimeFrameListBox = (gcnew System::Windows::Forms::ListBox());
			this->ReloadButton = (gcnew System::Windows::Forms::Button());
			this->availableBalanceHeading = (gcnew System::Windows::Forms::Label());
			this->orderBookHeading = (gcnew System::Windows::Forms::Label());
			this->priceHeading = (gcnew System::Windows::Forms::Label());
			this->marginHeading = (gcnew System::Windows::Forms::Label());
			this->orderTypeHeading = (gcnew System::Windows::Forms::Label());
			this->symbolsHeading = (gcnew System::Windows::Forms::Label());
			this->leverageHeading = (gcnew System::Windows::Forms::Label());
			this->aiBotHeading = (gcnew System::Windows::Forms::Label());
			this->MarkPriceHeading = (gcnew System::Windows::Forms::Label());
			this->openPositionsHeading = (gcnew System::Windows::Forms::Label());
			this->openOrdersHeading = (gcnew System::Windows::Forms::Label());
			this->aiTradeStatusHeading = (gcnew System::Windows::Forms::Label());
			this->aiTimeFrameHeading = (gcnew System::Windows::Forms::Label());
			this->positionSideValue = (gcnew System::Windows::Forms::Label());
			this->positionAmountHeading = (gcnew System::Windows::Forms::Label());
			this->positionLeverageHeading = (gcnew System::Windows::Forms::Label());
			this->positionPnlHeading = (gcnew System::Windows::Forms::Label());
			this->orderQuantityHeading = (gcnew System::Windows::Forms::Label());
			this->orderSideHeading = (gcnew System::Windows::Forms::Label());
			this->orderSymbolHeading = (gcnew System::Windows::Forms::Label());
			this->orderPriceHeading = (gcnew System::Windows::Forms::Label());
			this->positionQuantityHeading = (gcnew System::Windows::Forms::Label());
			this->positionSideHeading = (gcnew System::Windows::Forms::Label());
			this->positionSymbolHeading = (gcnew System::Windows::Forms::Label());
			this->positionEntryPriceHeading = (gcnew System::Windows::Forms::Label());
			this->tradeStatusBarHeading = (gcnew System::Windows::Forms::Label());
			this->marginValue = (gcnew System::Windows::Forms::Label());
			this->symbolSelectionMsg = (gcnew System::Windows::Forms::Label());
			this->orderTypeSelectionMsg = (gcnew System::Windows::Forms::Label());
			this->openChartButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->leverageSlider))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->marginSlider))->BeginInit();
			this->SuspendLayout();
			// 
			// symbolsListBox
			// 
			this->symbolsListBox->FormattingEnabled = true;
			this->symbolsListBox->Location = System::Drawing::Point(39, 46);
			this->symbolsListBox->Margin = System::Windows::Forms::Padding(2);
			this->symbolsListBox->Name = L"symbolsListBox";
			this->symbolsListBox->Size = System::Drawing::Size(180, 69);
			this->symbolsListBox->TabIndex = 0;
			this->symbolsListBox->Click += gcnew System::EventHandler(this, &MyForm::listBox1_Click);
			this->symbolsListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox1_SelectedIndexChanged);
			// 
			// leverageValue
			// 
			this->leverageValue->AutoSize = true;
			this->leverageValue->Location = System::Drawing::Point(334, 34);
			this->leverageValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->leverageValue->Name = L"leverageValue";
			this->leverageValue->Size = System::Drawing::Size(64, 13);
			this->leverageValue->TabIndex = 2;
			this->leverageValue->Text = L"LEVERAGE";
			// 
			// leverageSlider
			// 
			this->leverageSlider->Enabled = false;
			this->leverageSlider->Location = System::Drawing::Point(263, 56);
			this->leverageSlider->Margin = System::Windows::Forms::Padding(2);
			this->leverageSlider->Minimum = 1;
			this->leverageSlider->Name = L"leverageSlider";
			this->leverageSlider->Size = System::Drawing::Size(94, 45);
			this->leverageSlider->TabIndex = 3;
			this->leverageSlider->Value = 1;
			this->leverageSlider->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar1_Scroll);
			// 
			// availableBalanceValue
			// 
			this->availableBalanceValue->AutoSize = true;
			this->availableBalanceValue->Location = System::Drawing::Point(163, 143);
			this->availableBalanceValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->availableBalanceValue->Name = L"availableBalanceValue";
			this->availableBalanceValue->Size = System::Drawing::Size(64, 13);
			this->availableBalanceValue->TabIndex = 4;
			this->availableBalanceValue->Text = L"AVAILABLE";
			// 
			// OrderTypeListBox
			// 
			this->OrderTypeListBox->FormattingEnabled = true;
			this->OrderTypeListBox->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"LIMIT", L"MARKET" });
			this->OrderTypeListBox->Location = System::Drawing::Point(53, 260);
			this->OrderTypeListBox->Margin = System::Windows::Forms::Padding(2);
			this->OrderTypeListBox->Name = L"OrderTypeListBox";
			this->OrderTypeListBox->Size = System::Drawing::Size(97, 30);
			this->OrderTypeListBox->TabIndex = 5;
			this->OrderTypeListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox2_SelectedIndexChanged);
			// 
			// priceTextBox
			// 
			this->priceTextBox->Enabled = false;
			this->priceTextBox->Location = System::Drawing::Point(122, 331);
			this->priceTextBox->Margin = System::Windows::Forms::Padding(2);
			this->priceTextBox->Name = L"priceTextBox";
			this->priceTextBox->Size = System::Drawing::Size(87, 20);
			this->priceTextBox->TabIndex = 6;
			this->priceTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::textBox1_KeyPress);
			// 
			// marginSlider
			// 
			this->marginSlider->Enabled = false;
			this->marginSlider->Location = System::Drawing::Point(185, 260);
			this->marginSlider->Margin = System::Windows::Forms::Padding(2);
			this->marginSlider->Minimum = 1;
			this->marginSlider->Name = L"marginSlider";
			this->marginSlider->Size = System::Drawing::Size(172, 45);
			this->marginSlider->TabIndex = 8;
			this->marginSlider->Value = 1;
			this->marginSlider->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar2_Scroll);
			// 
			// buyButton
			// 
			this->buyButton->BackColor = System::Drawing::Color::ForestGreen;
			this->buyButton->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buyButton->ForeColor = System::Drawing::SystemColors::Control;
			this->buyButton->Location = System::Drawing::Point(54, 389);
			this->buyButton->Margin = System::Windows::Forms::Padding(2);
			this->buyButton->Name = L"buyButton";
			this->buyButton->Size = System::Drawing::Size(252, 40);
			this->buyButton->TabIndex = 9;
			this->buyButton->Text = L"BUY";
			this->buyButton->UseVisualStyleBackColor = false;
			this->buyButton->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// sellButton
			// 
			this->sellButton->BackColor = System::Drawing::Color::Red;
			this->sellButton->Font = (gcnew System::Drawing::Font(L"Times New Roman", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->sellButton->ForeColor = System::Drawing::SystemColors::Control;
			this->sellButton->Location = System::Drawing::Point(54, 441);
			this->sellButton->Margin = System::Windows::Forms::Padding(2);
			this->sellButton->Name = L"sellButton";
			this->sellButton->Size = System::Drawing::Size(252, 40);
			this->sellButton->TabIndex = 10;
			this->sellButton->Text = L"SELL";
			this->sellButton->UseVisualStyleBackColor = false;
			this->sellButton->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// orderBookValue1
			// 
			this->orderBookValue1->AutoSize = true;
			this->orderBookValue1->Location = System::Drawing::Point(479, 92);
			this->orderBookValue1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue1->Name = L"orderBookValue1";
			this->orderBookValue1->Size = System::Drawing::Size(89, 13);
			this->orderBookValue1->TabIndex = 11;
			this->orderBookValue1->Text = L"orderBookValue1";
			// 
			// orderBookValue2
			// 
			this->orderBookValue2->AutoSize = true;
			this->orderBookValue2->Location = System::Drawing::Point(558, 92);
			this->orderBookValue2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue2->Name = L"orderBookValue2";
			this->orderBookValue2->Size = System::Drawing::Size(89, 13);
			this->orderBookValue2->TabIndex = 12;
			this->orderBookValue2->Text = L"orderBookValue2";
			// 
			// orderBookValue3
			// 
			this->orderBookValue3->AutoSize = true;
			this->orderBookValue3->Location = System::Drawing::Point(479, 124);
			this->orderBookValue3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue3->Name = L"orderBookValue3";
			this->orderBookValue3->Size = System::Drawing::Size(89, 13);
			this->orderBookValue3->TabIndex = 13;
			this->orderBookValue3->Text = L"orderBookValue3";
			// 
			// orderBookValue4
			// 
			this->orderBookValue4->AutoSize = true;
			this->orderBookValue4->Location = System::Drawing::Point(558, 124);
			this->orderBookValue4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue4->Name = L"orderBookValue4";
			this->orderBookValue4->Size = System::Drawing::Size(89, 13);
			this->orderBookValue4->TabIndex = 14;
			this->orderBookValue4->Text = L"orderBookValue4";
			// 
			// orderBookValue5
			// 
			this->orderBookValue5->AutoSize = true;
			this->orderBookValue5->Location = System::Drawing::Point(479, 157);
			this->orderBookValue5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue5->Name = L"orderBookValue5";
			this->orderBookValue5->Size = System::Drawing::Size(89, 13);
			this->orderBookValue5->TabIndex = 15;
			this->orderBookValue5->Text = L"orderBookValue5";
			// 
			// orderBookValue6
			// 
			this->orderBookValue6->AutoSize = true;
			this->orderBookValue6->Location = System::Drawing::Point(558, 157);
			this->orderBookValue6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue6->Name = L"orderBookValue6";
			this->orderBookValue6->Size = System::Drawing::Size(89, 13);
			this->orderBookValue6->TabIndex = 16;
			this->orderBookValue6->Text = L"orderBookValue6";
			// 
			// MarkPriceValue
			// 
			this->MarkPriceValue->AutoSize = true;
			this->MarkPriceValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->MarkPriceValue->Location = System::Drawing::Point(489, 331);
			this->MarkPriceValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->MarkPriceValue->Name = L"MarkPriceValue";
			this->MarkPriceValue->Size = System::Drawing::Size(88, 16);
			this->MarkPriceValue->TabIndex = 17;
			this->MarkPriceValue->Text = L"MARK PRICE";
			// 
			// orderBookValue7
			// 
			this->orderBookValue7->AutoSize = true;
			this->orderBookValue7->Location = System::Drawing::Point(479, 188);
			this->orderBookValue7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue7->Name = L"orderBookValue7";
			this->orderBookValue7->Size = System::Drawing::Size(89, 13);
			this->orderBookValue7->TabIndex = 18;
			this->orderBookValue7->Text = L"orderBookValue7";
			// 
			// orderBookValue8
			// 
			this->orderBookValue8->AutoSize = true;
			this->orderBookValue8->Location = System::Drawing::Point(558, 188);
			this->orderBookValue8->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue8->Name = L"orderBookValue8";
			this->orderBookValue8->Size = System::Drawing::Size(89, 13);
			this->orderBookValue8->TabIndex = 19;
			this->orderBookValue8->Text = L"orderBookValue8";
			// 
			// orderBookValue9
			// 
			this->orderBookValue9->AutoSize = true;
			this->orderBookValue9->Location = System::Drawing::Point(479, 220);
			this->orderBookValue9->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue9->Name = L"orderBookValue9";
			this->orderBookValue9->Size = System::Drawing::Size(89, 13);
			this->orderBookValue9->TabIndex = 20;
			this->orderBookValue9->Text = L"orderBookValue9";
			// 
			// orderBookValue10
			// 
			this->orderBookValue10->AutoSize = true;
			this->orderBookValue10->Location = System::Drawing::Point(558, 220);
			this->orderBookValue10->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookValue10->Name = L"orderBookValue10";
			this->orderBookValue10->Size = System::Drawing::Size(95, 13);
			this->orderBookValue10->TabIndex = 21;
			this->orderBookValue10->Text = L"orderBookValue10";
			// 
			// positionAmountValue
			// 
			this->positionAmountValue->AutoSize = true;
			this->positionAmountValue->Location = System::Drawing::Point(558, 596);
			this->positionAmountValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionAmountValue->Name = L"positionAmountValue";
			this->positionAmountValue->Size = System::Drawing::Size(106, 13);
			this->positionAmountValue->TabIndex = 39;
			this->positionAmountValue->Text = L"positionAmountValue";
			// 
			// positionEntryPriceValue
			// 
			this->positionEntryPriceValue->AutoSize = true;
			this->positionEntryPriceValue->Location = System::Drawing::Point(558, 563);
			this->positionEntryPriceValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionEntryPriceValue->Name = L"positionEntryPriceValue";
			this->positionEntryPriceValue->Size = System::Drawing::Size(118, 13);
			this->positionEntryPriceValue->TabIndex = 38;
			this->positionEntryPriceValue->Text = L"positionEntryPriceValue";
			// 
			// positionLeverageValue
			// 
			this->positionLeverageValue->AutoSize = true;
			this->positionLeverageValue->Location = System::Drawing::Point(560, 534);
			this->positionLeverageValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionLeverageValue->Name = L"positionLeverageValue";
			this->positionLeverageValue->Size = System::Drawing::Size(115, 13);
			this->positionLeverageValue->TabIndex = 37;
			this->positionLeverageValue->Text = L"positionLeverageValue";
			// 
			// positionPnlValue
			// 
			this->positionPnlValue->AutoSize = true;
			this->positionPnlValue->Location = System::Drawing::Point(560, 509);
			this->positionPnlValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionPnlValue->Name = L"positionPnlValue";
			this->positionPnlValue->Size = System::Drawing::Size(85, 13);
			this->positionPnlValue->TabIndex = 36;
			this->positionPnlValue->Text = L"positionPnlValue";
			// 
			// positionQuantityValue
			// 
			this->positionQuantityValue->AutoSize = true;
			this->positionQuantityValue->Location = System::Drawing::Point(560, 476);
			this->positionQuantityValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionQuantityValue->Name = L"positionQuantityValue";
			this->positionQuantityValue->Size = System::Drawing::Size(109, 13);
			this->positionQuantityValue->TabIndex = 35;
			this->positionQuantityValue->Text = L"positionQuantityValue";
			// 
			// positionSymbolValue
			// 
			this->positionSymbolValue->AutoSize = true;
			this->positionSymbolValue->Location = System::Drawing::Point(560, 450);
			this->positionSymbolValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionSymbolValue->Name = L"positionSymbolValue";
			this->positionSymbolValue->Size = System::Drawing::Size(104, 13);
			this->positionSymbolValue->TabIndex = 34;
			this->positionSymbolValue->Text = L"positionSymbolValue";
			// 
			// Next_Position
			// 
			this->Next_Position->Location = System::Drawing::Point(539, 686);
			this->Next_Position->Name = L"Next_Position";
			this->Next_Position->Size = System::Drawing::Size(99, 24);
			this->Next_Position->TabIndex = 40;
			this->Next_Position->Text = L"Next Position>>";
			this->Next_Position->UseVisualStyleBackColor = true;
			this->Next_Position->Click += gcnew System::EventHandler(this, &MyForm::Next_Position_Click);
			// 
			// orderSymbolValue
			// 
			this->orderSymbolValue->AutoSize = true;
			this->orderSymbolValue->Location = System::Drawing::Point(119, 639);
			this->orderSymbolValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderSymbolValue->Name = L"orderSymbolValue";
			this->orderSymbolValue->Size = System::Drawing::Size(92, 13);
			this->orderSymbolValue->TabIndex = 44;
			this->orderSymbolValue->Text = L"orderSymbolValue";
			// 
			// orderPriceValue
			// 
			this->orderPriceValue->AutoSize = true;
			this->orderPriceValue->Location = System::Drawing::Point(119, 608);
			this->orderPriceValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderPriceValue->Name = L"orderPriceValue";
			this->orderPriceValue->Size = System::Drawing::Size(82, 13);
			this->orderPriceValue->TabIndex = 43;
			this->orderPriceValue->Text = L"orderPriceValue";
			// 
			// orderQuantityValue
			// 
			this->orderQuantityValue->AutoSize = true;
			this->orderQuantityValue->Location = System::Drawing::Point(119, 576);
			this->orderQuantityValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderQuantityValue->Name = L"orderQuantityValue";
			this->orderQuantityValue->Size = System::Drawing::Size(97, 13);
			this->orderQuantityValue->TabIndex = 42;
			this->orderQuantityValue->Text = L"orderQuantityValue";
			// 
			// orderSideValue
			// 
			this->orderSideValue->AutoSize = true;
			this->orderSideValue->Location = System::Drawing::Point(119, 546);
			this->orderSideValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderSideValue->Name = L"orderSideValue";
			this->orderSideValue->Size = System::Drawing::Size(79, 13);
			this->orderSideValue->TabIndex = 41;
			this->orderSideValue->Text = L"orderSideValue";
			// 
			// nextOrderButton
			// 
			this->nextOrderButton->Location = System::Drawing::Point(211, 576);
			this->nextOrderButton->Name = L"nextOrderButton";
			this->nextOrderButton->Size = System::Drawing::Size(95, 24);
			this->nextOrderButton->TabIndex = 45;
			this->nextOrderButton->Text = L"Next Order >>";
			this->nextOrderButton->UseVisualStyleBackColor = true;
			this->nextOrderButton->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
			// 
			// aiValueStorage1
			// 
			this->aiValueStorage1->AutoSize = true;
			this->aiValueStorage1->Enabled = false;
			this->aiValueStorage1->Location = System::Drawing::Point(846, 686);
			this->aiValueStorage1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiValueStorage1->Name = L"aiValueStorage1";
			this->aiValueStorage1->Size = System::Drawing::Size(85, 13);
			this->aiValueStorage1->TabIndex = 46;
			this->aiValueStorage1->Text = L"aiValueStorage1";
			this->aiValueStorage1->Visible = false;
			this->aiValueStorage1->TextChanged += gcnew System::EventHandler(this, &MyForm::label37_TextChanged);
			// 
			// TradeStatusBar
			// 
			this->TradeStatusBar->AutoSize = true;
			this->TradeStatusBar->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->TradeStatusBar->Location = System::Drawing::Point(771, 350);
			this->TradeStatusBar->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->TradeStatusBar->Name = L"TradeStatusBar";
			this->TradeStatusBar->Size = System::Drawing::Size(102, 16);
			this->TradeStatusBar->TabIndex = 47;
			this->TradeStatusBar->Text = L"TradeStatusBar";
			// 
			// aiValueStorage2
			// 
			this->aiValueStorage2->AutoSize = true;
			this->aiValueStorage2->Enabled = false;
			this->aiValueStorage2->Location = System::Drawing::Point(846, 666);
			this->aiValueStorage2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiValueStorage2->Name = L"aiValueStorage2";
			this->aiValueStorage2->Size = System::Drawing::Size(85, 13);
			this->aiValueStorage2->TabIndex = 48;
			this->aiValueStorage2->Text = L"aiValueStorage2";
			this->aiValueStorage2->Visible = false;
			this->aiValueStorage2->TextChanged += gcnew System::EventHandler(this, &MyForm::label39_TextChanged);
			// 
			// aiTradeStatusBar
			// 
			this->aiTradeStatusBar->AutoSize = true;
			this->aiTradeStatusBar->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->aiTradeStatusBar->Location = System::Drawing::Point(745, 305);
			this->aiTradeStatusBar->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiTradeStatusBar->Name = L"aiTradeStatusBar";
			this->aiTradeStatusBar->Size = System::Drawing::Size(113, 16);
			this->aiTradeStatusBar->TabIndex = 49;
			this->aiTradeStatusBar->Text = L"aiTradeStatusBar";
			// 
			// aiValueStorage3
			// 
			this->aiValueStorage3->AutoSize = true;
			this->aiValueStorage3->Enabled = false;
			this->aiValueStorage3->Location = System::Drawing::Point(846, 653);
			this->aiValueStorage3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiValueStorage3->Name = L"aiValueStorage3";
			this->aiValueStorage3->Size = System::Drawing::Size(85, 13);
			this->aiValueStorage3->TabIndex = 50;
			this->aiValueStorage3->Text = L"aiValueStorage3";
			this->aiValueStorage3->Visible = false;
			this->aiValueStorage3->TextChanged += gcnew System::EventHandler(this, &MyForm::label41_TextChanged);
			// 
			// aiValueStorage4
			// 
			this->aiValueStorage4->AutoSize = true;
			this->aiValueStorage4->Enabled = false;
			this->aiValueStorage4->Location = System::Drawing::Point(846, 678);
			this->aiValueStorage4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiValueStorage4->Name = L"aiValueStorage4";
			this->aiValueStorage4->Size = System::Drawing::Size(85, 13);
			this->aiValueStorage4->TabIndex = 51;
			this->aiValueStorage4->Text = L"aiValueStorage4";
			this->aiValueStorage4->Visible = false;
			this->aiValueStorage4->TextChanged += gcnew System::EventHandler(this, &MyForm::label42_TextChanged);
			// 
			// cancelOrderButton
			// 
			this->cancelOrderButton->Location = System::Drawing::Point(66, 686);
			this->cancelOrderButton->Name = L"cancelOrderButton";
			this->cancelOrderButton->Size = System::Drawing::Size(100, 24);
			this->cancelOrderButton->TabIndex = 52;
			this->cancelOrderButton->Text = L"Cancel Order";
			this->cancelOrderButton->UseVisualStyleBackColor = true;
			this->cancelOrderButton->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
			// 
			// closePositionButton
			// 
			this->closePositionButton->Location = System::Drawing::Point(420, 686);
			this->closePositionButton->Name = L"closePositionButton";
			this->closePositionButton->Size = System::Drawing::Size(99, 24);
			this->closePositionButton->TabIndex = 56;
			this->closePositionButton->Text = L"Close Position";
			this->closePositionButton->UseVisualStyleBackColor = true;
			this->closePositionButton->Click += gcnew System::EventHandler(this, &MyForm::button8_Click);
			// 
			// onScreenLogs
			// 
			this->onScreenLogs->Location = System::Drawing::Point(752, 101);
			this->onScreenLogs->Multiline = true;
			this->onScreenLogs->Name = L"onScreenLogs";
			this->onScreenLogs->ReadOnly = true;
			this->onScreenLogs->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->onScreenLogs->Size = System::Drawing::Size(217, 160);
			this->onScreenLogs->TabIndex = 57;
			this->onScreenLogs->TextChanged += gcnew System::EventHandler(this, &MyForm::textBox2_TextChanged);
			// 
			// aiCheckBox
			// 
			this->aiCheckBox->AutoSize = true;
			this->aiCheckBox->Enabled = false;
			this->aiCheckBox->Location = System::Drawing::Point(807, 480);
			this->aiCheckBox->Name = L"aiCheckBox";
			this->aiCheckBox->Size = System::Drawing::Size(65, 17);
			this->aiCheckBox->TabIndex = 58;
			this->aiCheckBox->Text = L"Turn On";
			this->aiCheckBox->UseVisualStyleBackColor = true;
			this->aiCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MyForm::checkBox1_CheckedChanged);
			// 
			// aiTimeFrameListBox
			// 
			this->aiTimeFrameListBox->Enabled = false;
			this->aiTimeFrameListBox->FormattingEnabled = true;
			this->aiTimeFrameListBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"1 Minute", L"1 Hour", L"1 Day" });
			this->aiTimeFrameListBox->Location = System::Drawing::Point(814, 552);
			this->aiTimeFrameListBox->Name = L"aiTimeFrameListBox";
			this->aiTimeFrameListBox->Size = System::Drawing::Size(91, 69);
			this->aiTimeFrameListBox->TabIndex = 59;
			this->aiTimeFrameListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::listBox3_SelectedIndexChanged);
			// 
			// ReloadButton
			// 
			this->ReloadButton->Location = System::Drawing::Point(880, 28);
			this->ReloadButton->Name = L"ReloadButton";
			this->ReloadButton->Size = System::Drawing::Size(89, 29);
			this->ReloadButton->TabIndex = 60;
			this->ReloadButton->Text = L"Reload";
			this->ReloadButton->UseVisualStyleBackColor = true;
			this->ReloadButton->Click += gcnew System::EventHandler(this, &MyForm::button9_Click);
			// 
			// availableBalanceHeading
			// 
			this->availableBalanceHeading->AutoSize = true;
			this->availableBalanceHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->availableBalanceHeading->Location = System::Drawing::Point(36, 141);
			this->availableBalanceHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->availableBalanceHeading->Name = L"availableBalanceHeading";
			this->availableBalanceHeading->Size = System::Drawing::Size(130, 15);
			this->availableBalanceHeading->TabIndex = 70;
			this->availableBalanceHeading->Text = L"Available Balance: ";
			// 
			// orderBookHeading
			// 
			this->orderBookHeading->AutoSize = true;
			this->orderBookHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->orderBookHeading->Location = System::Drawing::Point(451, 46);
			this->orderBookHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderBookHeading->Name = L"orderBookHeading";
			this->orderBookHeading->Size = System::Drawing::Size(80, 15);
			this->orderBookHeading->TabIndex = 71;
			this->orderBookHeading->Text = L"Order Book";
			// 
			// priceHeading
			// 
			this->priceHeading->AutoSize = true;
			this->priceHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->priceHeading->Location = System::Drawing::Point(51, 333);
			this->priceHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->priceHeading->Name = L"priceHeading";
			this->priceHeading->Size = System::Drawing::Size(49, 15);
			this->priceHeading->TabIndex = 72;
			this->priceHeading->Text = L"Price: ";
			// 
			// marginHeading
			// 
			this->marginHeading->AutoSize = true;
			this->marginHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->marginHeading->Location = System::Drawing::Point(197, 224);
			this->marginHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->marginHeading->Name = L"marginHeading";
			this->marginHeading->Size = System::Drawing::Size(59, 15);
			this->marginHeading->TabIndex = 73;
			this->marginHeading->Text = L"Margin: ";
			// 
			// orderTypeHeading
			// 
			this->orderTypeHeading->AutoSize = true;
			this->orderTypeHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->orderTypeHeading->Location = System::Drawing::Point(50, 234);
			this->orderTypeHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderTypeHeading->Name = L"orderTypeHeading";
			this->orderTypeHeading->Size = System::Drawing::Size(86, 15);
			this->orderTypeHeading->TabIndex = 74;
			this->orderTypeHeading->Text = L"Order Type: ";
			// 
			// symbolsHeading
			// 
			this->symbolsHeading->AutoSize = true;
			this->symbolsHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->symbolsHeading->Location = System::Drawing::Point(36, 22);
			this->symbolsHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->symbolsHeading->Name = L"symbolsHeading";
			this->symbolsHeading->Size = System::Drawing::Size(69, 15);
			this->symbolsHeading->TabIndex = 75;
			this->symbolsHeading->Text = L"Symbols: ";
			// 
			// leverageHeading
			// 
			this->leverageHeading->AutoSize = true;
			this->leverageHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->leverageHeading->Location = System::Drawing::Point(255, 32);
			this->leverageHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->leverageHeading->Name = L"leverageHeading";
			this->leverageHeading->Size = System::Drawing::Size(75, 15);
			this->leverageHeading->TabIndex = 76;
			this->leverageHeading->Text = L"Leverage: ";
			// 
			// aiBotHeading
			// 
			this->aiBotHeading->AutoSize = true;
			this->aiBotHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->aiBotHeading->Location = System::Drawing::Point(781, 441);
			this->aiBotHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiBotHeading->Name = L"aiBotHeading";
			this->aiBotHeading->Size = System::Drawing::Size(52, 15);
			this->aiBotHeading->TabIndex = 77;
			this->aiBotHeading->Text = L"AI Bot: ";
			// 
			// MarkPriceHeading
			// 
			this->MarkPriceHeading->AutoSize = true;
			this->MarkPriceHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 8.25F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->MarkPriceHeading->Location = System::Drawing::Point(415, 335);
			this->MarkPriceHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->MarkPriceHeading->Name = L"MarkPriceHeading";
			this->MarkPriceHeading->Size = System::Drawing::Size(70, 12);
			this->MarkPriceHeading->TabIndex = 78;
			this->MarkPriceHeading->Text = L"Mark Price: ";
			// 
			// openPositionsHeading
			// 
			this->openPositionsHeading->AutoSize = true;
			this->openPositionsHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->openPositionsHeading->Location = System::Drawing::Point(434, 421);
			this->openPositionsHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->openPositionsHeading->Name = L"openPositionsHeading";
			this->openPositionsHeading->Size = System::Drawing::Size(111, 15);
			this->openPositionsHeading->TabIndex = 79;
			this->openPositionsHeading->Text = L"Open Positions: ";
			// 
			// openOrdersHeading
			// 
			this->openOrdersHeading->AutoSize = true;
			this->openOrdersHeading->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->openOrdersHeading->Location = System::Drawing::Point(51, 511);
			this->openOrdersHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->openOrdersHeading->Name = L"openOrdersHeading";
			this->openOrdersHeading->Size = System::Drawing::Size(93, 15);
			this->openOrdersHeading->TabIndex = 80;
			this->openOrdersHeading->Text = L"Open Orders:";
			// 
			// aiTradeStatusHeading
			// 
			this->aiTradeStatusHeading->AutoSize = true;
			this->aiTradeStatusHeading->Location = System::Drawing::Point(658, 350);
			this->aiTradeStatusHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiTradeStatusHeading->Name = L"aiTradeStatusHeading";
			this->aiTradeStatusHeading->Size = System::Drawing::Size(106, 13);
			this->aiTradeStatusHeading->TabIndex = 90;
			this->aiTradeStatusHeading->Text = L"AI Bot Trade Status: ";
			// 
			// aiTimeFrameHeading
			// 
			this->aiTimeFrameHeading->AutoSize = true;
			this->aiTimeFrameHeading->Location = System::Drawing::Point(793, 519);
			this->aiTimeFrameHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->aiTimeFrameHeading->Name = L"aiTimeFrameHeading";
			this->aiTimeFrameHeading->Size = System::Drawing::Size(62, 13);
			this->aiTimeFrameHeading->TabIndex = 89;
			this->aiTimeFrameHeading->Text = L"TimeFrame:";
			// 
			// positionSideValue
			// 
			this->positionSideValue->AutoSize = true;
			this->positionSideValue->Location = System::Drawing::Point(558, 627);
			this->positionSideValue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionSideValue->Name = L"positionSideValue";
			this->positionSideValue->Size = System::Drawing::Size(91, 13);
			this->positionSideValue->TabIndex = 88;
			this->positionSideValue->Text = L"positionSideValue";
			// 
			// positionAmountHeading
			// 
			this->positionAmountHeading->AutoSize = true;
			this->positionAmountHeading->Location = System::Drawing::Point(467, 598);
			this->positionAmountHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionAmountHeading->Name = L"positionAmountHeading";
			this->positionAmountHeading->Size = System::Drawing::Size(49, 13);
			this->positionAmountHeading->TabIndex = 87;
			this->positionAmountHeading->Text = L"Amount: ";
			// 
			// positionLeverageHeading
			// 
			this->positionLeverageHeading->AutoSize = true;
			this->positionLeverageHeading->Location = System::Drawing::Point(467, 536);
			this->positionLeverageHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionLeverageHeading->Name = L"positionLeverageHeading";
			this->positionLeverageHeading->Size = System::Drawing::Size(58, 13);
			this->positionLeverageHeading->TabIndex = 86;
			this->positionLeverageHeading->Text = L"Leverage: ";
			// 
			// positionPnlHeading
			// 
			this->positionPnlHeading->AutoSize = true;
			this->positionPnlHeading->Location = System::Drawing::Point(468, 511);
			this->positionPnlHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionPnlHeading->Name = L"positionPnlHeading";
			this->positionPnlHeading->Size = System::Drawing::Size(34, 13);
			this->positionPnlHeading->TabIndex = 85;
			this->positionPnlHeading->Text = L"PNL: ";
			// 
			// orderQuantityHeading
			// 
			this->orderQuantityHeading->AutoSize = true;
			this->orderQuantityHeading->Location = System::Drawing::Point(59, 608);
			this->orderQuantityHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderQuantityHeading->Name = L"orderQuantityHeading";
			this->orderQuantityHeading->Size = System::Drawing::Size(52, 13);
			this->orderQuantityHeading->TabIndex = 84;
			this->orderQuantityHeading->Text = L"Quantity: ";
			// 
			// orderSideHeading
			// 
			this->orderSideHeading->AutoSize = true;
			this->orderSideHeading->Location = System::Drawing::Point(59, 639);
			this->orderSideHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderSideHeading->Name = L"orderSideHeading";
			this->orderSideHeading->Size = System::Drawing::Size(34, 13);
			this->orderSideHeading->TabIndex = 83;
			this->orderSideHeading->Text = L"Side: ";
			// 
			// orderSymbolHeading
			// 
			this->orderSymbolHeading->AutoSize = true;
			this->orderSymbolHeading->Location = System::Drawing::Point(59, 546);
			this->orderSymbolHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderSymbolHeading->Name = L"orderSymbolHeading";
			this->orderSymbolHeading->Size = System::Drawing::Size(47, 13);
			this->orderSymbolHeading->TabIndex = 82;
			this->orderSymbolHeading->Text = L"Symbol: ";
			// 
			// orderPriceHeading
			// 
			this->orderPriceHeading->AutoSize = true;
			this->orderPriceHeading->Location = System::Drawing::Point(59, 576);
			this->orderPriceHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->orderPriceHeading->Name = L"orderPriceHeading";
			this->orderPriceHeading->Size = System::Drawing::Size(37, 13);
			this->orderPriceHeading->TabIndex = 81;
			this->orderPriceHeading->Text = L"Price: ";
			// 
			// positionQuantityHeading
			// 
			this->positionQuantityHeading->AutoSize = true;
			this->positionQuantityHeading->Location = System::Drawing::Point(467, 478);
			this->positionQuantityHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionQuantityHeading->Name = L"positionQuantityHeading";
			this->positionQuantityHeading->Size = System::Drawing::Size(52, 13);
			this->positionQuantityHeading->TabIndex = 94;
			this->positionQuantityHeading->Text = L"Quantity: ";
			// 
			// positionSideHeading
			// 
			this->positionSideHeading->AutoSize = true;
			this->positionSideHeading->Location = System::Drawing::Point(467, 629);
			this->positionSideHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionSideHeading->Name = L"positionSideHeading";
			this->positionSideHeading->Size = System::Drawing::Size(34, 13);
			this->positionSideHeading->TabIndex = 93;
			this->positionSideHeading->Text = L"Side: ";
			// 
			// positionSymbolHeading
			// 
			this->positionSymbolHeading->AutoSize = true;
			this->positionSymbolHeading->Location = System::Drawing::Point(467, 452);
			this->positionSymbolHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionSymbolHeading->Name = L"positionSymbolHeading";
			this->positionSymbolHeading->Size = System::Drawing::Size(47, 13);
			this->positionSymbolHeading->TabIndex = 92;
			this->positionSymbolHeading->Text = L"Symbol: ";
			// 
			// positionEntryPriceHeading
			// 
			this->positionEntryPriceHeading->AutoSize = true;
			this->positionEntryPriceHeading->Location = System::Drawing::Point(467, 565);
			this->positionEntryPriceHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->positionEntryPriceHeading->Name = L"positionEntryPriceHeading";
			this->positionEntryPriceHeading->Size = System::Drawing::Size(64, 13);
			this->positionEntryPriceHeading->TabIndex = 91;
			this->positionEntryPriceHeading->Text = L"Entry Price: ";
			// 
			// tradeStatusBarHeading
			// 
			this->tradeStatusBarHeading->AutoSize = true;
			this->tradeStatusBarHeading->Location = System::Drawing::Point(658, 305);
			this->tradeStatusBarHeading->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->tradeStatusBarHeading->Name = L"tradeStatusBarHeading";
			this->tradeStatusBarHeading->Size = System::Drawing::Size(74, 13);
			this->tradeStatusBarHeading->TabIndex = 95;
			this->tradeStatusBarHeading->Text = L"Trade Status: ";
			// 
			// marginValue
			// 
			this->marginValue->AutoSize = true;
			this->marginValue->Location = System::Drawing::Point(274, 226);
			this->marginValue->Name = L"marginValue";
			this->marginValue->Size = System::Drawing::Size(69, 13);
			this->marginValue->TabIndex = 96;
			this->marginValue->Text = L"Margin Value";
			// 
			// symbolSelectionMsg
			// 
			this->symbolSelectionMsg->AutoSize = true;
			this->symbolSelectionMsg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->symbolSelectionMsg->ForeColor = System::Drawing::Color::Red;
			this->symbolSelectionMsg->Location = System::Drawing::Point(274, 114);
			this->symbolSelectionMsg->Name = L"symbolSelectionMsg";
			this->symbolSelectionMsg->Size = System::Drawing::Size(148, 13);
			this->symbolSelectionMsg->TabIndex = 98;
			this->symbolSelectionMsg->Text = L"Please Select a Symbol. ";
			// 
			// orderTypeSelectionMsg
			// 
			this->orderTypeSelectionMsg->AutoSize = true;
			this->orderTypeSelectionMsg->ForeColor = System::Drawing::Color::Red;
			this->orderTypeSelectionMsg->Location = System::Drawing::Point(36, 191);
			this->orderTypeSelectionMsg->Name = L"orderTypeSelectionMsg";
			this->orderTypeSelectionMsg->Size = System::Drawing::Size(114, 13);
			this->orderTypeSelectionMsg->TabIndex = 99;
			this->orderTypeSelectionMsg->Text = L"Select an Order Type. ";
			// 
			// openChartButton
			// 
			this->openChartButton->Enabled = false;
			this->openChartButton->Font = (gcnew System::Drawing::Font(L"Arial Rounded MT Bold", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->openChartButton->Location = System::Drawing::Point(741, 27);
			this->openChartButton->Name = L"openChartButton";
			this->openChartButton->Size = System::Drawing::Size(114, 30);
			this->openChartButton->TabIndex = 100;
			this->openChartButton->Text = L"Open Chart";
			this->openChartButton->UseVisualStyleBackColor = true;
			this->openChartButton->Click += gcnew System::EventHandler(this, &MyForm::button5_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1056, 780);
			this->Controls->Add(this->openChartButton);
			this->Controls->Add(this->orderTypeSelectionMsg);
			this->Controls->Add(this->symbolSelectionMsg);
			this->Controls->Add(this->marginValue);
			this->Controls->Add(this->tradeStatusBarHeading);
			this->Controls->Add(this->positionQuantityHeading);
			this->Controls->Add(this->positionSideHeading);
			this->Controls->Add(this->positionSymbolHeading);
			this->Controls->Add(this->positionEntryPriceHeading);
			this->Controls->Add(this->aiTradeStatusHeading);
			this->Controls->Add(this->aiTimeFrameHeading);
			this->Controls->Add(this->positionSideValue);
			this->Controls->Add(this->positionAmountHeading);
			this->Controls->Add(this->positionLeverageHeading);
			this->Controls->Add(this->positionPnlHeading);
			this->Controls->Add(this->orderQuantityHeading);
			this->Controls->Add(this->orderSideHeading);
			this->Controls->Add(this->orderSymbolHeading);
			this->Controls->Add(this->orderPriceHeading);
			this->Controls->Add(this->openOrdersHeading);
			this->Controls->Add(this->openPositionsHeading);
			this->Controls->Add(this->MarkPriceHeading);
			this->Controls->Add(this->aiBotHeading);
			this->Controls->Add(this->leverageHeading);
			this->Controls->Add(this->symbolsHeading);
			this->Controls->Add(this->orderTypeHeading);
			this->Controls->Add(this->marginHeading);
			this->Controls->Add(this->priceHeading);
			this->Controls->Add(this->orderBookHeading);
			this->Controls->Add(this->availableBalanceHeading);
			this->Controls->Add(this->ReloadButton);
			this->Controls->Add(this->aiTimeFrameListBox);
			this->Controls->Add(this->aiCheckBox);
			this->Controls->Add(this->onScreenLogs);
			this->Controls->Add(this->closePositionButton);
			this->Controls->Add(this->cancelOrderButton);
			this->Controls->Add(this->aiValueStorage4);
			this->Controls->Add(this->aiValueStorage3);
			this->Controls->Add(this->aiTradeStatusBar);
			this->Controls->Add(this->aiValueStorage2);
			this->Controls->Add(this->TradeStatusBar);
			this->Controls->Add(this->aiValueStorage1);
			this->Controls->Add(this->nextOrderButton);
			this->Controls->Add(this->orderSymbolValue);
			this->Controls->Add(this->orderPriceValue);
			this->Controls->Add(this->orderQuantityValue);
			this->Controls->Add(this->orderSideValue);
			this->Controls->Add(this->Next_Position);
			this->Controls->Add(this->positionAmountValue);
			this->Controls->Add(this->positionEntryPriceValue);
			this->Controls->Add(this->positionLeverageValue);
			this->Controls->Add(this->positionPnlValue);
			this->Controls->Add(this->positionQuantityValue);
			this->Controls->Add(this->positionSymbolValue);
			this->Controls->Add(this->orderBookValue10);
			this->Controls->Add(this->orderBookValue9);
			this->Controls->Add(this->orderBookValue8);
			this->Controls->Add(this->orderBookValue7);
			this->Controls->Add(this->MarkPriceValue);
			this->Controls->Add(this->orderBookValue6);
			this->Controls->Add(this->orderBookValue5);
			this->Controls->Add(this->orderBookValue4);
			this->Controls->Add(this->orderBookValue3);
			this->Controls->Add(this->orderBookValue2);
			this->Controls->Add(this->orderBookValue1);
			this->Controls->Add(this->sellButton);
			this->Controls->Add(this->buyButton);
			this->Controls->Add(this->marginSlider);
			this->Controls->Add(this->priceTextBox);
			this->Controls->Add(this->OrderTypeListBox);
			this->Controls->Add(this->availableBalanceValue);
			this->Controls->Add(this->leverageSlider);
			this->Controls->Add(this->leverageValue);
			this->Controls->Add(this->symbolsListBox);
			this->Margin = System::Windows::Forms::Padding(2);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->leverageSlider))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->marginSlider))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ALl the code above this is auto generated
	private: System::Void listBox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {      //Event handler for the symbol list box. Auto Generated. Is invoked when a symbol is selected.
		symbolSelectionMsg->Text = "";
		if (symbolsListBox->SelectedItem->ToString() != "Try Again") {         //If the selected item is not "Try Again" then proceed.
			if (!openChartButton->Enabled) {                 //If the open chart button is disabled then enable it.
				openChartButton->Enabled = true;
			}
			if (!leverageSlider->Enabled) {                //If the leverage trackbar is disabled then enable it.
				leverageSlider->Enabled = true;
			}
			std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());           //Convert the selected item to a string.
			std::string maxLeverage = fetchAllowedLeverage(symbol);                     //Fetch the maximum leverage allowed for the selected symbol.
			if(maxLeverage!= "Error occurred. Try Reloading. "){                //If the maximum leverage is not an error message then proceed.
				int leverage;
				leverage = std::stoi(maxLeverage);                     //Convert the maximum leverage to an integer.
				onScreenLogs->Text += "Leverage Fetched Successfully.\n";          //If the conversion is successful then display a success message.
				leverageSlider->Maximum = leverage;
				leverageSlider->Value = leverage;
				leverageValue->Text = gcnew String(maxLeverage.c_str());
			}
			else {
				onScreenLogs->Text += "Error: Couldn't Fetch Leverage. Try Again.\n";         //If the conversion is unsuccessful then display an error message.
			}
		}
	}

	private: System::Void trackBar1_Scroll(System::Object^ sender, System::EventArgs^ e) {        //Event handler for the leverage trackbar. Auto Generated. Is invoked when the trackbar is scrolled.
		leverageValue->Text = "" + leverageSlider->Value;            //Display the value of the trackbar in the label.
	}

	private: System::Void trackBar2_Scroll(System::Object^ sender, System::EventArgs^ e) {            //Event handler for the quantity trackbar. Auto Generated. Is invoked when the trackbar is scrolled.
		marginValue->Text = "" + marginSlider->Value;                   //Display the value of the trackbar in the label.
	}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {         //Event handler for the long trade button. Auto Generated. Is invoked when the button is clicked.
		placeLongTrade(0);                               //Call the placeLongTrade function.
	}

	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {             //Event handler for the short trade button. Auto Generated. Is invoked when the button is clicked.
		placeShortTrade(0);                        //Call the placeShortTrade function.
	}

		   //Creating variables to implement logic .
	private: int positionsStartIndex = 1;
	private: int positionsEndIndex = 7;
	private: int positionsArraySize = 0;
	private: int ordersArraySize = 1;
	private: int ordersStartIndex = 0;
	private: int ordersEndIndex = 5;
	private: bool cancelThisOrder = false;
	private: bool closeThisPosition = false;
	private: int x = 0;

	private: System::Void MyForm_Shown(System::Object^ sender, System::EventArgs^ e) {     //Event handler for the form. Auto Generated. Is invoked when the form is shown.
		onScreenLogs->Text = "";
		buyButton->FlatStyle=FlatStyle::Flat;
		buyButton->FlatAppearance->BorderSize =0;                                 //For removing the border of the buyButton
		buyButton->FlatAppearance->BorderColor = Color::FromArgb(0, 255, 255, 255);
		sellButton->FlatStyle = FlatStyle::Flat;
		sellButton->FlatAppearance->BorderSize = 0;                            //For removing the border of the sellButton
		sellButton->FlatAppearance->BorderColor = Color::FromArgb(0, 255, 255, 255);
		try{
			std::vector<std::string> symbols = fetchAvailableSymbols();                   //Fetch the available symbols.
			if (symbols[0] == "Reload") {                          //If the first element of the vector is "Reload" then display an error message.
				onScreenLogs->Text += "Error: Couldn't Fetch Available Symbols. Try Again.\n";
			}
			else {                                              //Else display a success message and add the symbols to the list box.
				onScreenLogs->Text += "Available Symbols Fetched Successfully.\n";
				for (const auto& symbol : symbols) {
					symbolsListBox->Items->Add(gcnew String(symbol.c_str()));          //Displaying the symbols in the list box.
				}
			}
		}
		catch(const std::exception& e){
			onScreenLogs->Text += "Error: Couldn't Fetch Available Symbols. Try Again.\n";           //If the conversion is unsuccessful then display an error message.
		}
		//Initializing the labels
		leverageValue->Text = "";
		positionSideValue->Text = "";
		availableBalanceValue->Text = "";
		positionSymbolValue->Text = "";
		positionQuantityValue->Text = "";
		positionPnlValue->Text = "";
		positionLeverageValue->Text = "";
		positionEntryPriceValue->Text = "";
		positionAmountValue->Text = "";
		orderSideValue->Text = "";
		orderQuantityValue->Text = "";
		orderPriceValue->Text = "";
		orderSymbolValue->Text = "";
		orderBookValue1->Text = "";
		orderBookValue2->Text = "";
		orderBookValue3->Text = "";
		orderBookValue4->Text = "";
		orderBookValue5->Text = "";
		orderBookValue6->Text = "";
		orderBookValue7->Text = "";
		orderBookValue8->Text = "";
		orderBookValue9->Text = "";
		orderBookValue10->Text = "";
		TradeStatusBar->Text = "";
		aiTradeStatusBar->Text = "";
		marginValue->Text = "";
		MarkPriceValue->Text = "";
	}

	private: System::Void listBox1_Click(System::Object^ sender, System::EventArgs^ e) {       //Event handler for the symbol list box. Auto Generated. Is invoked when the list box is clicked.
		if (symbolsListBox->SelectedIndex != -1 && msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString()) != "Reload") {
			aiCheckBox->Enabled = true;           //If the selected item is not "Reload" then enable the checkbox for AI.
		}
	}

	private: void positionsInitializer(int i) {                            //Function to initialize and show the open positions. Also has the logic for closing the positions.
		std::vector<std::string> open_positions = fetchOpenPositions();          //Fetch the open positions.
		if (closeThisPosition && open_positions.size()>1) {
			std::string symbol = open_positions[1];                         //If the closeThisPosition flag is true then close the position.
			if (i == -1) {
				i = 1;
				symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());              //Fetching the value of the selected symbol
				int index = findIndex(open_positions, symbol);                              //Finding the index of the selected symbol in the open positions vector.
				if (index == -1) {
					MarkPriceValue->Text="Position Not Found. Try Again.";                        //If the index is -1 then display an error message.
					onScreenLogs->Text+="Error: Couldn't Close Position. Try Again.\n";
				}
				else {
					i = index;
				}
			}
			if (open_positions.size() > 1) {
				
				double positionAmt = std::stod(open_positions[i + 5]);          //If the open positions vector is not empty then proceed. Here we are extracting values to close the position.
				double currentPrice;              
				try {
					currentPrice = std::stod(fetchRealTimeValue(symbol));                 //Fetching the current price of the selected symbol.
				}
				catch (const std::exception) {
					aiTradeStatusBar->Text = "Couldn't close the position. Try Again.";                 //If the conversion is unsuccessful then display an error message.
					aiTradeStatusBar->ForeColor= Color::Red;
					return;
				}
				int leverage = std::stoi(open_positions[i + 3]);                             //Fetching the leverage of the selected symbol.
				std::string result = closePosition(symbol, positionAmt, currentPrice, leverage);            //Closing the position.
				aiTradeStatusBar->Text = gcnew String(result.c_str());
				closeThisPosition = false;
				if (result == "Trade closed successfully!") {               //If the position is closed successfully then display a success message.
					aiTradeStatusBar->Text= "Position Closed Successfully.";
					aiTradeStatusBar->ForeColor = Color::Green;
					onScreenLogs->Text+="Position Closed Successfully.\n";
					open_positions = fetchOpenPositions();
				}
				else {
					aiTradeStatusBar->Text = "Couldn't close the position. Try Again.";            //If the position is not closed successfully then display an error message.
					aiTradeStatusBar->ForeColor = Color::Red;
					onScreenLogs->Text+="Error: Couldn't Close Position. Try Again.\n";
				}
				i = 1;
			}
			else {
				aiTradeStatusBar->Text = "No Open Positions";           //Else no positions are open
				aiTradeStatusBar->ForeColor = Color::Red;
			}
		}

		positionsArraySize = open_positions.size() - 1;
		if (open_positions[0] != "Try Reloading") {

			try {
				int availableBalance = std::stoi(open_positions[0]);           //The 0 index of the open_positions array is the available balance that we will display
				onScreenLogs->Text += "Open Positions Fetched Successfully.\n";
			}
			catch(const std::exception& e){                   //Catch Error and display message accordingly
				onScreenLogs->Text+= gcnew String("Error: Couldn't Fetch Open Positions. \n");
				return;
			}
			availableBalanceValue->Text = gcnew String(std::to_string(std::stoi(open_positions[0])).c_str());         //Display the available balance
			marginSlider->Maximum = std::stoi(open_positions[0]);                   //Set maximum value of the track Bar2 to the available balance
		}
		if (positionsArraySize) {				//If the open positions vector is not empty then proceed.
			int pnl = std::stoi(open_positions[i + 2]);                  //Retrieving the PNL (Profit N Loss) to display its color accordingly
			positionSymbolValue->Text = gcnew String(open_positions[i].c_str());         //Displaying Symbol
			positionQuantityValue->Text = gcnew String(std::to_string(std::stoi(open_positions[i + 1])).c_str());         
			positionPnlValue->Text = gcnew String(std::to_string(pnl).c_str());
			if(pnl>0)      //If profit. Color is green otherwise color is red
				positionPnlValue->ForeColor = Color::Green;
			else
				positionPnlValue->ForeColor = Color::Red;
			positionLeverageValue->Text = gcnew String(open_positions[i + 3].c_str());
			positionEntryPriceValue->Text = gcnew String(std::to_string(std::stoi(open_positions[i + 4])).c_str());
			double positionAmt = std::stod(open_positions[i + 5]);      //Extracting position amount for displaying colors
			positionAmountValue->Text = gcnew String(open_positions[i + 5].c_str());
			if(positionAmt>0)            
			{
				positionSideValue->Text = "LONG";           //Displaying side 
				positionSideValue->ForeColor = Color::Green;   //Displaying color
			}
			else
			{
				positionSideValue->Text = "SHORT";
				positionSideValue->ForeColor = Color::Red;
			}
		}
		else {
			onScreenLogs->Text+= "No Open Positions.\n";           //If no positions are open then display a message."
			positionSymbolValue->Text = "No Open Positions";
			positionQuantityValue->Text = "";
			positionPnlValue->Text = "";
			positionLeverageValue->Text = "";
			positionEntryPriceValue->Text = "";
			positionAmountValue->Text = "";
			positionSideValue->Text = "";
		}
	}

	private: void ordersInitializer(int k) {
		std::vector<std::string> open_orders = fetchOpenOrders();           //Fetching Open Orders
		if (cancelThisOrder && open_orders.size() > 1 && open_orders[0]!="Press the Next Button or Reload. ") {             //Checking conditions to figure out if we have to cancel order or not
			unsigned int orderId = std::stoul(open_orders[k + 4]);          //Extracting Order Id from Open Orders array
			std::string result = cancelOrder(orderId, open_orders[k]);         //Cancelling order
			aiTradeStatusBar->Text = gcnew String(result.c_str());
			cancelThisOrder = false;
			if (result == "Try Again After Reloading. ") {                //Showing error msg with appropriate color if occurs
				onScreenLogs->Text += "Error: Couldn't Cancel Order. Try Again.\n";
				aiTradeStatusBar->ForeColor=Color::Red;
			}
			else {                              //Shows result with green color
				aiTradeStatusBar->ForeColor = Color::Green;
				open_orders = fetchOpenOrders();
			}
		}
		if (open_orders[0] == "Press the Next Button or Reload. ") {                //Showing error msg with appropriate color if occurs
			onScreenLogs->Text+="Error: Couldn't Fetch Open Orders. Try Again.\n";
			return;
		}
		else {
			onScreenLogs->Text+="Open Orders Fetched Successfully.\n";       //Showing msg
		}
		ordersArraySize = open_orders.size() - 1;
		try {                              //Checking limit1 to figure out which order to display .Considering that we have a next order button
			if (ordersArraySize && open_orders[0]!="Open Orders Fetched Successfully!") {
				orderSideValue->Text = gcnew String(open_orders[k].c_str());
				orderQuantityValue->Text = gcnew String(open_orders[k + 1].c_str());
				orderPriceValue->Text = gcnew String(open_orders[k + 2].c_str());
				orderSymbolValue->Text = gcnew String(open_orders[k + 3].c_str());
				if (msclr::interop::marshal_as<std::string>(orderSymbolValue->Text->ToString()) == "BUY") {         //Showing the side in Red or Green depending upon the side of the order
					orderSymbolValue->ForeColor = Color::Green;
				}
				else {
					orderSymbolValue->ForeColor = Color::Red;
				}
			}
			else {
				orderSideValue->Text = "No Open Orders";       //Showing that no orders are open
				orderQuantityValue->Text = "";
				orderPriceValue->Text = "";
				orderSymbolValue->Text = "";
			}
		}
		catch(const std::exception& e){        //Catching unexpected errors
			onScreenLogs->Text+="Unexpected Error: Couldn't Fetch Open Orders. Try Again.\n";
		}
	}

	private: void aiRunner() {
		x++;                 //This Functions main Task is to gether the parameters required to run the AI
		std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());         //Extracts symbol
		std::string timeFrame = msclr::interop::marshal_as<std::string>(aiTimeFrameListBox->SelectedItem->ToString());     //Extracts TimeFrame
		if (timeFrame == "1 Minute")
			timeFrame = "1m";
		else if (timeFrame == "1 Hour")
			timeFrame = "1h";
		else if (timeFrame == "1 Day")
			timeFrame = "1d";
		std::vector<double> hsitoricalData = fetchHistoricalData(timeFrame, symbol);            //Fetches historical data for Training of the AI
		std::vector<bool> trades = ai(hsitoricalData);              //Passes it to the AI
		if (trades.size() != 0) {
			aiValueStorage1->Text = trades[0] ? "true" : "false";
			aiValueStorage3->Text = trades[1] ? "true" : "false";                //It is difficult to pass variables or values accross different threads so here we are using an ingenious way. 
			aiValueStorage2->Text = trades[2] ? "true" : "false";               // We make these labels invisible and we also disable them. Then we store values into these labels in one thread.
			aiValueStorage4->Text = trades[3] ? "true" : "false";               // And access those values from the other thread
		}
	}

	private: void placeLongTrade(int x) {      //Function for placing LONG Trade
		//Buy
		if (symbolsListBox->SelectedIndex == -1) {            //If symbol not selected
			aiTradeStatusBar->Text = "Please Select a Symbol";
			aiTradeStatusBar->ForeColor = Color::Red;
		}
		else {
			std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());           //Extract symbol name
			if (x) {             //Logic to figure out wheter to place a trade for the AI or for the user
				std::string availableBalance = msclr::interop::marshal_as<std::string>(availableBalanceValue->Text->ToString());     //fetching the available balance
				double quantity = std::stod(availableBalance)*0.1;            //Uses only 10% of the portfolio for the AI trades
				int leverage = leverageSlider->Value;
				std::string result = placeTrade(symbol, "BUY", quantity, "MARKET", 0, leverage);        //Placing trade
				aiTradeStatusBar->Text = gcnew String(result.c_str());
				if (result == "Trade placed successfully!") {        //showing the result in green if successful
					aiTradeStatusBar->ForeColor = Color::Green;
					onScreenLogs->Text+="Trade Placed Successfully.\n";
				}
				else {              //showing in red if failure
					aiTradeStatusBar->ForeColor = Color::Red;
					onScreenLogs->Text += "Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
					return;
				}
			}
			else if (OrderTypeListBox->SelectedIndex == -1) {         //Checking if an order type is selected
				aiTradeStatusBar->Text = "Please Select an  Order Type";
				aiTradeStatusBar->ForeColor = Color::Red;
			}
			else {
				std::string orderType = msclr::interop::marshal_as<std::string>(OrderTypeListBox->SelectedItem->ToString());          //If yes then retrieving it
				if (leverageSlider->Value * marginSlider->Value < 10) {           //Checking if the amount for trade selected by the user is appropriate
					aiTradeStatusBar->Text = "Either Increase the Leverage or the Margin. ";
					aiTradeStatusBar->ForeColor = Color::Red;
				}
				else {
					int leverage = leverageSlider->Value;             //Retreiving leverage
					int margin = marginSlider->Value;               //Retreiing the margin selected
					std::string realtimeValue = fetchRealTimeValue(symbol);        //Fetching the realtime value
					if (OrderTypeListBox->SelectedItem == "LIMIT") {        //Checking what type of trade order was selected
						if (priceTextBox->Text == "") {
							aiTradeStatusBar->Text = "Please Enter a Price";           //Showing response
							aiTradeStatusBar->ForeColor = Color::Red;
						}
						else {
							double price = std::stod(msclr::interop::marshal_as<std::string>(priceTextBox->Text->ToString()));         //Fetching price selected
							double quantity = margin / price;         //Calculating quantity
							std::string result = placeTrade(symbol, "BUY", quantity, orderType, price, leverage);       //Placing Trade
							onScreenLogs->Text += gcnew String(result.c_str());
							if(result== "Trade placed successfully!")         //Showing result in green if successful 
							{
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								aiTradeStatusBar->ForeColor = Color::Green;
							}
							else if (result == "Order's notional must be no smaller than 100 (unless you choose reduce only).") {
								aiTradeStatusBar->Text = "Increase the Margin. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else if (result == "The counterparty's best price does not meet the PERCENT_PRICE filter limit.") {
								//For sending the correct response back to the user
								aiTradeStatusBar->Text = "Exchange Does not have enough liquidity for this symbol at the moment. Try Again Later. ";
								aiTradeStatusBar->ForeColor = Color::Red;	
							}
							else {
								aiTradeStatusBar->Text = gcnew String(result.c_str());         //Otherwise in red. 
								onScreenLogs->Text+="Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
								aiTradeStatusBar->ForeColor = Color::Red;
								return;
							}
							positionsInitializer(positionsStartIndex);           //Fetching the new Positions
							ordersInitializer(ordersStartIndex);        //Fetching the new orders
						}
					}
					else if (OrderTypeListBox->SelectedItem == "MARKET") {
						if (realtimeValue == "Try Again After Reloading. ") {             //Trying to retrieve current price of the symbol
							aiTradeStatusBar->Text = gcnew String(realtimeValue.c_str());
							aiTradeStatusBar->ForeColor = Color::Red;
						}
						else {
							double quantity = margin / std::stod(realtimeValue);      //Calculating quantity
							std::string result = placeTrade(symbol, "BUY", quantity, orderType, 0, leverage);         //Placing trade
							onScreenLogs->Text += gcnew String(result.c_str());
							if (result == "Trade placed successfully!")               //Show result in green upon success
							{
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								aiTradeStatusBar->ForeColor = Color::Green;
							}
							else if (result == "Order's notional must be no smaller than 100 (unless you choose reduce only).") {
								aiTradeStatusBar->Text = "Increase the Margin. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else if (result == "The counterparty's best price does not meet the PERCENT_PRICE filter limit.") {
								//For sending the correct response back to the user
								aiTradeStatusBar->Text = "Exchange Does not have enough liquidity for this symbol at the moment. Try Again Later. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else {
								aiTradeStatusBar->Text = gcnew String(result.c_str());               //otherwise in red
								onScreenLogs->Text += "Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
								aiTradeStatusBar->ForeColor = Color::Red;
								return;
							}
							positionsInitializer(positionsStartIndex);            //Fetching only the orders because here this is the only possibity
						}
					}
					else {
						aiTradeStatusBar->Text = "Please Select an Order Type";
						aiTradeStatusBar->ForeColor = Color::Red;
					}
				}
			}
		}
	}

	private: void placeShortTrade(int x) {           //For placing SHORT or SELL trades
		//SELL
		if (symbolsListBox->SelectedIndex == -1) {           //Checking if correct symbol is selected
			aiTradeStatusBar->Text = "Please Select a Symbol";
			aiTradeStatusBar->ForeColor = Color::Red;
		}
		else {
			std::string symbol = msclr::interop::marshal_as<std::string>(symbolsListBox->SelectedItem->ToString());       //Extracting symbol
			if (x) {
				std::string availableBalance = msclr::interop::marshal_as<std::string>(availableBalanceValue->Text->ToString());      //extracting avalible balance
				double quantity = std::stod(availableBalance) * 0.1;
				int leverage = leverageSlider->Value;                         //extracting leverage
				std::string result = placeTrade(symbol, "SELL", quantity, "MARKET", 0, leverage);        //Placing the SHORT trade
				aiTradeStatusBar->Text = gcnew String(result.c_str());
				if (result == "Trade placed successfully!") {          //Displaying result in green if successful
					aiTradeStatusBar->ForeColor = Color::Green;
					onScreenLogs->Text+="Trade Placed Successfully.\n";
				}
				else {
					aiTradeStatusBar->ForeColor = Color::Red;            //Otherwise in red
					onScreenLogs->Text += "Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
					return;
				}
			}
			else if (OrderTypeListBox->SelectedIndex == -1) {         //Checking if correct order type is selected
				aiTradeStatusBar->Text = "Please Select an  Order Type";
				aiTradeStatusBar->ForeColor = Color::Red;
			}
			else {          
				std::string orderType = msclr::interop::marshal_as<std::string>(OrderTypeListBox->SelectedItem->ToString());        //Extracting the selected order type
				if (leverageSlider->Value * marginSlider->Value < 6) {            //Using this because the quantity of trade should be greater than 6
					aiTradeStatusBar->Text = "Either Increase the Leverage or the Margin. ";
					aiTradeStatusBar->ForeColor = Color::Red;
				}
				else {
					int leverage = leverageSlider->Value;        //Extracting leverage
					int margin = marginSlider->Value;          //Extracting margin
					std::string realtimeValue = fetchRealTimeValue(symbol);           //Fetching realtime value of the symbol
					if (OrderTypeListBox->SelectedItem == "LIMIT") {         //Placing LIMIT order. 
						if (priceTextBox->Text == "") {          //Checking if price wala text box is empty or not
							aiTradeStatusBar->Text = "Please Enter a Price";
							aiTradeStatusBar->ForeColor = Color::Red;
						}
						else {
							double price = stod(msclr::interop::marshal_as<std::string>(priceTextBox->Text->ToString()));      //Extracing price
							double quantity = margin / price;            //Calculating Quantity
							std::string result= placeTrade(symbol, "SELL", quantity, orderType, price, leverage);              //Placing trade
							onScreenLogs->Text+=gcnew String(result.c_str());
							if (result == "Trade placed successfully!")     //Displaying result in green if successful
							{
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								aiTradeStatusBar->ForeColor = Color::Green;
							}
							else if (result == "Order's notional must be no smaller than 100 (unless you choose reduce only).") {
								aiTradeStatusBar->Text = "Increase the Margin. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else if (result == "The counterparty's best price does not meet the PERCENT_PRICE filter limit.") {
								//For sending the correct response back to the user
								aiTradeStatusBar->Text = "Exchange Does not have enough liquidity for this symbol at the moment. Try Again Later. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else {                //Otherwise in red
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								onScreenLogs->Text += "Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
								aiTradeStatusBar->ForeColor = Color::Red;
								return;
							}
							positionsInitializer(positionsStartIndex);              //Fetching Both the open orders and open positions again
							ordersInitializer(ordersStartIndex);
						}
					}
					else if (OrderTypeListBox->SelectedItem == "MARKET") {          //Placing market order
						if (realtimeValue == "Try Again After Reloading. ") {             //Checking if the realtime price was fetched successfully
							aiTradeStatusBar->Text = gcnew String(realtimeValue.c_str());
							aiTradeStatusBar->ForeColor = Color::Red;
						}
						else {
							double quantity = margin / std::stod(realtimeValue);              //Calculating the quantity
							std::string result =placeTrade(symbol, "SELL", quantity, orderType, 0, leverage);          //Placing the short trade
							onScreenLogs->Text+=gcnew String(result.c_str());
							if (result == "Trade placed successfully!") //Displaying result in green upon success
							{
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								aiTradeStatusBar->ForeColor = Color::Green;
							}
							else if (result == "Order's notional must be no smaller than 100 (unless you choose reduce only).") {
								aiTradeStatusBar->Text = "Increase the Margin. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else if (result == "The counterparty's best price does not meet the PERCENT_PRICE filter limit.") {
								//For sending the correct response back to the user
								aiTradeStatusBar->Text = "Exchange Does not have enough liquidity for this symbol at the moment. Try Again Later. ";
								aiTradeStatusBar->ForeColor = Color::Red;
							}
							else {                       //And in red upon failure
								aiTradeStatusBar->Text = gcnew String(result.c_str());
								onScreenLogs->Text += "Error: Couldn't Place Trade. See the Trade Status For More Info or Try Again.\n";
								aiTradeStatusBar->ForeColor = Color::Red;
								return;
							}
							positionsInitializer(positionsStartIndex);           //Because the MARKET order if fulfilled immediately and it has no chance of becoming an order. That is why there is no need to fetch the open 
						}                                   //orders and only fetching the open posiitons will suffice
					}
					else {
						aiTradeStatusBar->Text = "Please Select an Order Type";
						aiTradeStatusBar->ForeColor = Color::Red;
					}
				}
			}
		}
	}

	private: System::Void listBox2_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {    //This is an auto generated event handler that is invked when an item is selected from OrderTypeListBox
		if (!marginSlider->Enabled) {          //Enabes the margin trackBar if disabled
			orderTypeSelectionMsg->Text = "";
			marginSlider->Enabled = true;
		}
		std::string orderType = msclr::interop::marshal_as<std::string>(OrderTypeListBox->SelectedItem->ToString());          //Fetches order type and based on it enables or disables the price text box
		if(orderType=="LIMIT"&&!priceTextBox->Enabled)
			priceTextBox->Enabled = true;
		else if (orderType != "LIMIT" && priceTextBox->Enabled) {
			priceTextBox->Enabled = false;
		}
	}

	private: System::Void Next_Position_Click(System::Object^ sender, System::EventArgs^ e) {         //This is an auto generated event handler that is invoked when the next positon button is clicked

		if (positionsEndIndex == positionsArraySize + 1 || positionsEndIndex > positionsArraySize) {          //It uses this logic to determine which positions to display. The current one or the next one. If there is only one open position then 
			positionsStartIndex = 1;                              //even after pressing the button that will be displayed
			positionsEndIndex = 7;
		}
		else {
			positionsStartIndex += 6;
			positionsEndIndex += 6;
		}
		positionsInitializer(positionsStartIndex);        //Displays the open positions again to show the next position or the current position if number of open positions is 1 or 0

	}

	private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) {        //This is an auto generated event handler that is invoked when the next positon button is clicked
		if (ordersEndIndex == ordersArraySize || ordersEndIndex >= ordersArraySize) {          //It uses this logic to determine which positions to display. The current one or the next one. If there is only one open position then 
			ordersStartIndex = 0;                                       //even after pressing the button that will be displayed
			ordersEndIndex = 6;
		}
		else {
			ordersStartIndex += 5;
			ordersEndIndex += 5;
		}
		ordersInitializer(ordersStartIndex);         //Displays the open orders again to show the next order or the current order if number of open orders is 1 or 0
	}

	private: System::Void textBox1_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {    //This is an auto generated event handler that is invoked when something is written in the textBox 1. WHich is the price text box
		if (!Char::IsDigit(e->KeyChar) && e->KeyChar != '.' && e->KeyChar != '\b') {      //This is logic that only accepts numbers from 0-9 and the '.' decimal point as input in the text box field for the price
			e->Handled = true; // Ignore the character input
		}

		// Allow the backspace key
		if (e->KeyChar == '\b') {
			e->Handled = false;
		}
	}

	private: System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) {    //This is an auto generated event handler that is invoked when the cancel order button is clicked
		cancelThisOrder = true;		 //Sets the cancelThisOrder flag to true
		ordersInitializer(ordersStartIndex);		//Displays the open orders but since the cancelThisOrder flag is true it will cancel the order
	}

	private: System::Void button8_Click(System::Object^ sender, System::EventArgs^ e) {   //This is an auto generated event handler that is invoked when the close position button is clicked
		closeThisPosition = true;		//Sets the closeThisPosition flag to true
		positionsInitializer(positionsStartIndex);			//Displays the open positions but since the closeThisPosition flag is true it will close the position
	}

	private: System::Void textBox2_TextChanged(System::Object^ sender, System::EventArgs^ e) {      //This is an auto generated event handler that is invoked when something is written in the textBox 2. WHich is the log text box
		onScreenLogs->SelectionStart = onScreenLogs->Text->Length;  //This is logic that makes the text box scroll to the bottom automatically when something is written in it
		onScreenLogs->ScrollToCaret();           
	}

	private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {  //This is an auto generated event handler that is invoked when the AI checkbox is checked or unchecked
		if (aiCheckBox->Checked) {
			aiTimeFrameListBox->Enabled = true;       //Enables the time frame list box
		}
		else {
			aiTimeFrameListBox->Enabled = false;       //Disables the time frame list box
		}
	}

	private: System::Void listBox3_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {      //This is an auto generated event handler that is invoked when an item is selected from aiTimeFrameListBox
		System::Windows::Forms::DialogResult result = MessageBox::Show("The Setting Up of the AI may take some time. Do you want to continue?", "Run AI", MessageBoxButtons::YesNo, MessageBoxIcon::Question);
		if (result == System::Windows::Forms::DialogResult::Yes)         //Displays a message box to confirm if the user wants to run the AI
		{
			MessageBox::Show("The AI may take about 20 seconds to start. Do not close the program or your computer!", "Do not close the Program!", MessageBoxButtons::OK, MessageBoxIcon::Information);
			aiRunner();          //Runs the AI
		}
	}

	private: System::Void label37_TextChanged(System::Object^ sender, System::EventArgs^ e) {  //This is an auto generated event handler that is invoked when the aiValueStorage1 is changed
		if (msclr::interop::marshal_as<std::string>(aiValueStorage1->Text->ToString()) == "true") {
			TradeStatusBar->Text = "AI: Placing a LONG Order";       //If the aiValueStorage1 is true then it means that the AI has predicted that a LONG trade should be placed
			placeLongTrade(1);
		}
	}

	private: System::Void label41_TextChanged(System::Object^ sender, System::EventArgs^ e) {  //This is an auto generated event handler that is invoked when the aiValueStorage3 is changed
		if (msclr::interop::marshal_as<std::string>(aiValueStorage1->Text->ToString()) == "true") {
			closeThisPosition = true;
			TradeStatusBar->Text = "AI: Close LONG Trade. "; 	 //If the aiValueStorage3 is true then it means that the AI has predicted that a LONG trade should be closed
			positionsInitializer(-1);
		}
	}

	private: System::Void label39_TextChanged(System::Object^ sender, System::EventArgs^ e) {  //This is an auto generated event handler that is invoked when the aiValueStorage2 is changed
		if (msclr::interop::marshal_as<std::string>(aiValueStorage2->Text->ToString()) == "true") {
			TradeStatusBar->Text = "AI: Place SHORT Order";           //If the aiValueStorage2 is true then it means that the AI has predicted that a SHORT trade should be placed
			placeShortTrade(1);
		}
	}

	private: System::Void label42_TextChanged(System::Object^ sender, System::EventArgs^ e) {  //This is an auto generated event handler that is invoked when the aiValueStorage4 is changed
		if (msclr::interop::marshal_as<std::string>(aiValueStorage1->Text->ToString()) == "true") {
			TradeStatusBar->Text = "AI: Close SHORT Trade. ";
			cancelThisOrder = true;		   //If the aiValueStorage4 is true then it means that the AI has predicted that a SHORT trade should be closed
			positionsInitializer(-1);
		}
	}

	private: System::Void button9_Click(System::Object^ sender, System::EventArgs^ e) {     //This is an auto generated event handler that is invoked when the reload button is clicked
		MyForm^ newForm = gcnew MyForm();

		// Hide the current form instead of closing it
		this->Hide();

		// Show the new form
		newForm->Show();
	}

	private: System::Void button5_Click(System::Object^ sender, System::EventArgs^ e) {     //This is an auto generated event handler that is invoked when the reload button is clicked
		System::Diagnostics::Process::Start("https://gocharting.com/terminal?ticker=BINANCE:FUTURE:" + symbolsListBox->SelectedItem->ToString());
	}        //It opens the gocharting website with the selected symbol to show the chart of the symbol

	private: System::Void MyForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
		Application::ExitThread();
		Application::Exit();
	}
};
}
