//
// MainPage.xaml.h
// Объявление класса MainPage.
//
//old
#pragma once

#include "MainPage.g.h"

namespace line_characteristics_calculator
{
	/// <summary>
	/// Пустая страница, которую можно использовать саму по себе или для перехода внутри фрейма.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

		void fillCombo();
		void drawGraph();
		void MainPage::drawApproximatedCurve(float x_c, float y_c, float ax1, float ax2, float dec);
		void MainPage::drawApproximatedCurve_2(float a0, float a2, float a3);

		void calc_btn_click_callback();
		void select_a_begin_callback();
		void select_a_end_callback();
		void select_b_begin_callback();
		void select_b_end_callback();
		void select_c_begin_callback();
		void select_c_end_callback();
		void select_d_begin_callback();
		void select_d_end_callback();
		void select_e_begin_callback();
		void select_e_end_callback();
	private:
		logical::Calculator^ calculator;
		float shift = 400;
		float scale = 20;
		float radius = 100;

		void on_read_file_btn_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
