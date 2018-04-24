// validateitemdelegate.h -- ValidateItemDelegate class
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

// ISC License
// 
// Copyright 2018 Farid Hajji <farid@hajji.name>
// 
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted, provided
// that the above copyright notice and this permission notice appear
// in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
// OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <Wt/WItemDelegate.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WModelIndex.h>
#include <Wt/WFlags.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WAny.h>
#include <Wt/WValidator.h>

class ValidateItemDelegate : public Wt::WItemDelegate {
public:
	ValidateItemDelegate(std::shared_ptr<Wt::WValidator> validator) :
		Wt::WItemDelegate(),
		validator_(validator) {}

	std::unique_ptr<Wt::WWidget> createEditor(const Wt::WModelIndex& index,
			Wt::WFlags<Wt::ViewItemRenderFlag> flags) const
	{
		// Code copied from WItemDelegate (4.0.3), except for parts marked XXX
		// Also replaced IndexContainerWidget by WContainerWidget

		std::unique_ptr<Wt::WContainerWidget> result(new Wt::WContainerWidget());
		result->setSelectable(true);

		std::unique_ptr<Wt::WLineEdit> lineEdit(new Wt::WLineEdit());
		lineEdit->setText(asString(index.data(Wt::ItemDataRole::Edit), textFormat()));
		lineEdit->enterPressed().connect
		(this, std::bind(&ValidateItemDelegate::doCloseEditor, this, result.get(), true));
		lineEdit->escapePressed().connect
		(this, std::bind(&ValidateItemDelegate::doCloseEditor, this, result.get(), false));
		lineEdit->escapePressed().preventPropagation();

		if (flags.test(Wt::ViewItemRenderFlag::Focused))
			lineEdit->setFocus(true);

		lineEdit->resize(Wt::WLength(100, Wt::LengthUnit::Percentage),
			Wt::WLength(100, Wt::LengthUnit::Percentage)); //for Konqueror

		// XXX: attach validator to the lineEdit
		lineEdit->setValidator(validator_);

		result->addWidget(std::move(lineEdit));

		return std::move(result);
	}

	void doCloseEditor(Wt::WWidget *editor, bool save) const
	{
		// Code copied from WItemDelegate (4.0.3), except for parts marked XXX
		// Also replaced IndexContainerWidget by WContainerWidget

		// XXX: check validator before saving changes
		if (save) {
			Wt::WContainerWidget *w =
				dynamic_cast<Wt::WContainerWidget *>(editor);
			Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(w->widget(0));

			save = lineEdit->validate() == Wt::ValidationState::Valid;
		}

		// Original code
		closeEditor().emit(editor, save);
	}

	Wt::cpp17::any editState(Wt::WWidget *editor, const Wt::WModelIndex& /* index */)
		const
	{
		// Code copied from WItemDelegate (4.0.3), except for parts marked XXX
		// Also replaced IndexContainerWidget by WContainerWidget

		Wt::WContainerWidget *w =
			dynamic_cast<Wt::WContainerWidget *>(editor);
		Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(w->widget(0));

		return Wt::cpp17::any(lineEdit->text());
	}

	void setEditState(Wt::WWidget *editor, const Wt::WModelIndex& /* index */,
		const Wt::cpp17::any& value) const
	{
		// Code copied from WItemDelegate (4.0.3), except for parts marked XXX
		// Also replaced IndexContainerWidget by WContainerWidget

		Wt::WContainerWidget *w =
			dynamic_cast<Wt::WContainerWidget *>(editor);
		Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(w->widget(0));

		lineEdit->setText(Wt::cpp17::any_cast<WT_USTRING>(value));
	}

private:
	std::shared_ptr<Wt::WValidator> validator_;
};
