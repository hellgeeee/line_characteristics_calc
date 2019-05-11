//
// MainPage.xaml.cpp
// Реализация класса MainPage.
//
//old
#include "pch.h"
#include "MainPage.xaml.h"
#include <string>

using namespace line_characteristics_calculator; //??

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;
using namespace Windows::Storage;

using namespace std;
using namespace logical;

MainPage::MainPage()
{
	InitializeComponent();

	calculator = ref new Calculator();	

	TranslateTransform^ tran_tmp = ref new TranslateTransform(); 
	tran_tmp->X = -100; // сдвинем все объекты за область видимости
	a_begin_mark->RenderTransform = tran_tmp;
	a_end_mark->RenderTransform = tran_tmp;
	b_begin_mark->RenderTransform = tran_tmp;
	b_end_mark->RenderTransform = tran_tmp;
	c_begin_mark->RenderTransform = tran_tmp;
	c_end_mark->RenderTransform = tran_tmp;
	d_begin_mark->RenderTransform = tran_tmp;
	d_end_mark->RenderTransform = tran_tmp;
	e_begin_mark->RenderTransform = tran_tmp;
	e_end_mark->RenderTransform = tran_tmp;
	approximated_curv_line->RenderTransform = tran_tmp;
}

void MainPage::on_read_file_btn_click(Object^ sender, RoutedEventArgs^ e)
{ 
	using namespace Windows::Storage::Pickers;
	using namespace Windows::Storage::Streams;
	using namespace Windows::Storage;
	
	auto picker = ref new FileOpenPicker();
	picker->FileTypeFilter->Append(L".txt");
	picker->SuggestedStartLocation = PickerLocationId::Desktop;

	create_task(picker->PickSingleFileAsync()).then(
		[this](StorageFile^ in_file) {
		if (nullptr == in_file)
			return;
		create_task(in_file->OpenReadAsync()).then(
			[this](IRandomAccessStreamWithContentType^ stream) {


			UINT64 in_size = stream->Size; //сколько символов в файле
			String^ stream_text = stream->ToString();
			Windows::Storage::Streams::IInputStream^ input_stream = stream->GetInputStreamAt(0);
			auto data_reader = ref new DataReader(input_stream);

			create_task(data_reader->LoadAsync(in_size)).then([&, data_reader](unsigned int numBytesLoaded)
			{
				info_lbl->Text = "File is loading. Please, wait...";
				calculator->clear();
				OutputDebugString(L"1. Начало считывания файла\r\n");

				String^ in_text = data_reader->ReadString(numBytesLoaded);
				wstring in_text_conversed(in_text->Data());

				int i_pos = 0;
				wstring s_pos(L"");
				float x, y;
				while (true) {

					// для x
					i_pos = in_text_conversed.find(L" ");
					if (i_pos < 0) break; // ничего не нашли - выходим из цикла
					s_pos = in_text_conversed.substr(0, i_pos); // иначе берем все что до пробела
					x = std::stof(s_pos); // переводим в float
					in_text_conversed = in_text_conversed.substr(i_pos, in_size - i_pos); // и отрезаем это (больше не нужно). Хотя здесь может быть быстрее использовать счетчик будет

					// для y - те же действия, за исключением разделительного знака
					i_pos = in_text_conversed.find(L"\n");
					if (i_pos < 0) break; // ничего не нашли - выходим из цикла
					s_pos = in_text_conversed.substr(0, i_pos); // иначе берем все что до пробела
					y = std::stof(s_pos); // переводим в float
					in_text_conversed = in_text_conversed.substr(i_pos, in_size - i_pos); // и отрезаем это (больше не нужно). Хотя здесь может быть быстрее использовать счетчик будет

					calculator->add_point(Point(x, y));
				}
				OutputDebugString(L"1. Конец считывания файла\r\n");

				fillCombo();
				drawGraph();
				info_lbl->Text = "File was successfully loaded. Please, pick points and calculate";
			});
		});
	});

}

void MainPage::fillCombo() {

	// сначала почистим комбу
	a_begin_combo->Items->Clear();
	a_end_combo->Items->Clear();
	b_begin_combo->Items->Clear();
	b_end_combo->Items->Clear();
	c_begin_combo->Items->Clear();
	c_end_combo->Items->Clear();
	d_begin_combo->Items->Clear();
	d_end_combo->Items->Clear();
	e_begin_combo->Items->Clear();
	e_end_combo->Items->Clear();

	int input_size = calculator->get_input_size();
	for (int i = 0; i < input_size; i++) {

		float x = calculator->get_x_by_index(i);
		float y = calculator->get_y_by_index(i);
		String^ str_tmp = "(" + x + ", " + y + ")";
		a_begin_combo->Items->Append(str_tmp);
		a_end_combo->Items->Append(str_tmp);
		b_begin_combo->Items->Append(str_tmp);
		b_end_combo->Items->Append(str_tmp);
		c_begin_combo->Items->Append(str_tmp);
		c_end_combo->Items->Append(str_tmp);
		d_begin_combo->Items->Append(str_tmp);
		d_end_combo->Items->Append(str_tmp);
		e_begin_combo->Items->Append(str_tmp);
		e_end_combo->Items->Append(str_tmp);
	}
}

void MainPage::drawGraph() {
	data_graph->Points->Clear();
	int input_size = calculator->get_input_size();
	for (int i = 0; i < input_size; i++) {
		float x = calculator->get_x_by_index(i) * scale + shift;
		float y = calculator->get_y_by_index(i) * scale;
		data_graph->Points->Append(Point(x, y));
	}
}

void MainPage::drawApproximatedCurve(float x_c, float y_c, float ax1, float ax2, float dec) {

	// избавимся от предыдущего рисунка сначала
	approximated_curv_line_2->Points->Clear();

	approximated_curv_line->Width = 2 * ax1 * scale;
	approximated_curv_line->Height = 2 * ax2 * scale;

	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	// хотим перенести в центр именно центр эллипса, а не его верхний левый угол
	tran_tmp->X = x_c * scale + shift - ax1 * scale;
	tran_tmp->Y = y_c * scale - ax2 * scale;

	RotateTransform^ rot_tmp = ref new RotateTransform;
	rot_tmp->Angle = dec / 3.14 * 180;
	rot_tmp->CenterX = x_c * scale + shift;
	rot_tmp->CenterY = y_c * scale;

	TransformGroup^ transforms = ref new TransformGroup();
	transforms->Children->Append(tran_tmp);
	transforms->Children->Append(rot_tmp);

	approximated_curv_line->RenderTransform = transforms;

}

void MainPage::calc_btn_click_callback() {
	
	calc_btn->IsEnabled = false;
	info_lbl->Text = "Calculation is proceccing. Please, wait...";

	float tmp;
	IVector<float>^ f_tmp = ref new Platform::Collections::Vector<float>();;
	String^ s_tmp;

	tmp = calculator->calculateStep();
	if (tmp != _FAKE_VALUE) {
		s_tmp = "" + tmp;
		step_ans_lbl->Text = s_tmp;
	}
	else {
		step_ans_lbl->Text = "Initial data is not enough";
	}

	tmp = calculator->calculateLinesAngle();
	if (tmp != _FAKE_VALUE) {
		s_tmp = "" + tmp;
		angle_ans_lbl->Text = s_tmp;
	}
	else {
		angle_ans_lbl->Text = "Initial data is not enough";
	}

	f_tmp = calculator->calculateLinesCross();
	if (f_tmp->GetAt(0) != _FAKE_VALUE) {
		s_tmp = "(" + f_tmp->GetAt(0) + ", " + f_tmp->GetAt(1) + ")";
		crosspoint_ans_lbl->Text = s_tmp;
	} else {
		crosspoint_ans_lbl->Text = "Initial data is not enough";
	}
	
	if (method_combo->SelectedIndex < 0) {
		calc_btn->IsEnabled = true;
		info_lbl->Text = "Please choose method for curve approximation";
		return;
	}

	if (method_combo->SelectedIndex == 0) {
		calculator->calculateApproximatedCurve_bruteForceMethod();

		f_tmp = calculator->get_rez_curve();
		if (f_tmp->GetAt(0) != _FAKE_VALUE) {

			s_tmp = "(" + f_tmp->GetAt(0) + ", " + f_tmp->GetAt(1) + ")";
			ellipse_center_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(2) + " and " + f_tmp->GetAt(3);
			ellipse_cemiaxes_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(4);
			ellipse_decline_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(5);
			ellipse_excentricity_ans_lbl->Text = s_tmp;

		}
		else {
			ellipse_center_ans_lbl->Text = "Initial data is not enough";
			ellipse_cemiaxes_ans_lbl->Text = "Initial data is not enough";
			ellipse_decline_ans_lbl->Text = "Initial data is not enough";
			ellipse_excentricity_ans_lbl->Text = "Initial data is not enough";
		}

	} else if (method_combo->SelectedIndex == 1) { // кусочно-полиномиальный метод
		calculator->calculateApproximatedCurve_partialPolinomialMethod();
		f_tmp = calculator->get_rez_curve();

		ellipse_center_ans_lbl->Text = "You can not achieve this attitude with chosen method";
		ellipse_cemiaxes_ans_lbl->Text = "You can not achieve this attitude with chosen method";
		ellipse_decline_ans_lbl->Text = "You can not achieve this attitude with chosen method";
		ellipse_excentricity_ans_lbl->Text = "IYou can not achieve this attitude with chosen method";

	} else if (method_combo->SelectedIndex == 2) { // метод "в лоб", но с уточнениями

		calculator->calculateApproximatedCurve_bruteForceMethod_optimized();
		f_tmp = calculator->get_rez_curve();
		if (f_tmp->GetAt(0) != _FAKE_VALUE) {

			s_tmp = "(" + f_tmp->GetAt(0) + ", " + f_tmp->GetAt(1) + ")";
			ellipse_center_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(2) + " and " + f_tmp->GetAt(3);
			ellipse_cemiaxes_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(4);
			ellipse_decline_ans_lbl->Text = s_tmp;

			s_tmp = "" + f_tmp->GetAt(5);
			ellipse_excentricity_ans_lbl->Text = s_tmp;

		}
		else {
			ellipse_center_ans_lbl->Text = "Initial data is not enough";
			ellipse_cemiaxes_ans_lbl->Text = "Initial data is not enough";
			ellipse_decline_ans_lbl->Text = "Initial data is not enough";
			ellipse_excentricity_ans_lbl->Text = "Initial data is not enough";
		}

	}

	if (f_tmp->GetAt(0) != _FAKE_VALUE )
		drawApproximatedCurve(f_tmp->GetAt(0), f_tmp->GetAt(1), f_tmp->GetAt(2), f_tmp->GetAt(3), f_tmp->GetAt(4));
	calc_btn->IsEnabled = true;
	info_lbl->Text = "Calculation is finished";

}

void MainPage::select_a_begin_callback() {
	// запоминаем выбранную точку в калькулятор
	calculator->set_a_begin(a_begin_combo->SelectedIndex);

	// устанавливаем ее на поле
	float x1 = calculator->get_a_begin_x() * scale + shift;
	float y1 = calculator->get_a_begin_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x1; tran_tmp->Y = y1;
	a_begin_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (a_end_combo->SelectedIndex >= 0) {
		a_line->X1 = x1;
		a_line->Y1 = y1;
		a_line->X2 = calculator->get_a_end_x() * scale + shift;
		a_line->Y2 = calculator->get_a_end_y() * scale;
	}
}

void MainPage::select_a_end_callback() {
	calculator->set_a_end(a_end_combo->SelectedIndex);
	float x2 = calculator->get_a_end_x() * scale + shift;
	float y2 = calculator->get_a_end_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x2; tran_tmp->Y = y2;
	a_end_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (a_begin_combo->SelectedIndex >= 0) {
		a_line->X2 = x2;
		a_line->Y2 = y2;
		a_line->X1 = calculator->get_a_begin_x() * scale + shift;
		a_line->Y1 = calculator->get_a_begin_y() * scale;
	}
}

void MainPage::select_b_begin_callback() {
	// запоминаем выбранную точку в калькулятор
	calculator->set_b_begin(b_begin_combo->SelectedIndex);

	// устанавливаем ее на поле
	float x1 = calculator->get_b_begin_x() * scale + shift;
	float y1 = calculator->get_b_begin_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x1; tran_tmp->Y = y1;
	b_begin_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (b_end_combo->SelectedIndex >= 0) {
		b_line->X1 = x1;
		b_line->Y1 = y1;
		b_line->X2 = calculator->get_b_end_x() * scale + shift;
		b_line->Y2 = calculator->get_b_end_y() * scale;
	}
}

void MainPage::select_b_end_callback() {
	calculator->set_b_end(b_end_combo->SelectedIndex);
	float x2 = calculator->get_b_end_x() * scale + shift;
	float y2 = calculator->get_b_end_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x2; tran_tmp->Y = y2;
	b_end_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (a_begin_combo->SelectedIndex >= 0) {
		b_line->X2 = x2;
		b_line->Y2 = y2;
		b_line->X1 = calculator->get_b_begin_x() * scale + shift;
		b_line->Y1 = calculator->get_b_begin_y() * scale;
	}
}

void MainPage::select_c_begin_callback() {
	// запоминаем выбранную точку в калькулятор
	calculator->set_c_begin(c_begin_combo->SelectedIndex);

	// устанавливаем ее на поле
	float x1 = calculator->get_c_begin_x() * scale + shift;
	float y1 = calculator->get_c_begin_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x1; tran_tmp->Y = y1;
	c_begin_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (c_end_combo->SelectedIndex >= 0) {
		c_line->X1 = x1;
		c_line->Y1 = y1;
		c_line->X2 = calculator->get_c_end_x() * scale + shift;
		c_line->Y2 = calculator->get_c_end_y() * scale;
	}
}

void MainPage::select_c_end_callback() {
	calculator->set_c_end(c_end_combo->SelectedIndex);
	float x2 = calculator->get_c_end_x() * scale + shift;
	float y2 = calculator->get_c_end_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x2; tran_tmp->Y = y2;
	c_end_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (a_begin_combo->SelectedIndex >= 0) {
		c_line->X2 = x2;
		c_line->Y2 = y2;
		c_line->X1 = calculator->get_c_begin_x() * scale + shift;
		c_line->Y1 = calculator->get_c_begin_y() * scale;
	}
}

void MainPage::select_d_begin_callback() {
	// запоминаем выбранную точку в калькулятор
	calculator->set_d_begin(d_begin_combo->SelectedIndex);

	// устанавливаем ее на поле
	float x1 = calculator->get_d_begin_x() * scale + shift;
	float y1 = calculator->get_d_begin_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x1; tran_tmp->Y = y1;
	d_begin_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (d_end_combo->SelectedIndex >= 0) {
		d_line->X1 = x1;
		d_line->Y1 = y1;
		d_line->X2 = calculator->get_d_end_x() * scale + shift;
		d_line->Y2 = calculator->get_d_end_y() * scale;
	}
}

void MainPage::select_d_end_callback() {
	calculator->set_d_end(d_end_combo->SelectedIndex);
	float x2 = calculator->get_d_end_x() * scale + shift;
	float y2 = calculator->get_d_end_y() * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x2; tran_tmp->Y = y2;
	d_end_mark->RenderTransform = tran_tmp;

	// если точка с другого конца тоже выбрана, рисуем отрезок
	if (d_begin_combo->SelectedIndex >= 0) {
		d_line->X2 = x2;
		d_line->Y2 = y2;
		d_line->X1 = calculator->get_d_begin_x() * scale + shift;
		d_line->Y1 = calculator->get_d_begin_y() * scale;
	}
}

void MainPage::select_e_begin_callback() {
	float x1 = calculator->get_x_by_index(e_begin_combo->SelectedIndex) * scale + shift;
	float y1 = calculator->get_y_by_index(e_begin_combo->SelectedIndex) * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x1; tran_tmp->Y = y1;
	e_begin_mark->RenderTransform = tran_tmp;

	if (e_end_combo->SelectedIndex >= 0) {

		// расположить начальный и конечный индексы по возрастанию
		int i1, i2;
		if (e_begin_combo->SelectedIndex < e_end_combo->SelectedIndex) {
			i1 = e_begin_combo->SelectedIndex;
			i2 = e_end_combo->SelectedIndex;
		} else {
			i2 = e_begin_combo->SelectedIndex;
			i1 = e_end_combo->SelectedIndex;
		}

		// запомнить точки для эллипса
		calculator->set_e(i1, i2);

		// отобразить линию для элллипса
		e_line->Points->Clear();
		for (int i = i1; i <= i2; i++) {
			e_line->Points->Append( Point(calculator->get_x_by_index(i) * scale + shift, calculator->get_y_by_index(i) * scale) );
		}
	}
}

void MainPage::select_e_end_callback() {
	float x2 = calculator->get_x_by_index(e_end_combo->SelectedIndex) * scale + shift;
	float y2 = calculator->get_y_by_index(e_end_combo->SelectedIndex) * scale;
	TranslateTransform^ tran_tmp = ref new TranslateTransform();
	tran_tmp->X = x2; tran_tmp->Y = y2;
	e_end_mark->RenderTransform = tran_tmp;

	if ( e_begin_combo->SelectedIndex >= 0 ) {

		// расположить начальный и конечный индексы по возрастанию
		int i1, i2;
		if (e_begin_combo->SelectedIndex < e_end_combo->SelectedIndex) {
			i1 = e_begin_combo->SelectedIndex;
			i2 = e_end_combo->SelectedIndex;
		}
		else {
			i2 = e_begin_combo->SelectedIndex;
			i1 = e_end_combo->SelectedIndex;
		}

		// запомнить точки для эллипса
		calculator->set_e(i1, i2);

		// отобразить линию для элллипса
		e_line->Points->Clear();
		for (int i = i1; i <= i2; i++) {
			e_line->Points->Append(Point(calculator->get_x_by_index(i) * scale + shift, calculator->get_y_by_index(i) * scale));
		}
	}

}