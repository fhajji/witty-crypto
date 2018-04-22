// validateitemdelegate.h -- ValidateItemDelegate class
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

#pragma once

#include <Wt/WItemDelegate.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WModelIndex.h>
#include <Wt/WFlags.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WAny.h>

class ValidateItemDelegate : public Wt::WItemDelegate {
public:
	ValidateItemDelegate() : Wt::WItemDelegate() {}

	std::unique_ptr<Wt::WWidget> createEditor(const Wt::WModelIndex& index,
			Wt::WFlags<Wt::ViewItemRenderFlag> flags) const
	{
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
		result->addWidget(std::move(lineEdit));

		return std::move(result);
	}

	void doCloseEditor(Wt::WWidget *editor, bool save) const
	{
		closeEditor().emit(editor, save);
	}

	Wt::cpp17::any editState(Wt::WWidget *editor, const Wt::WModelIndex& index)
		const
	{
		Wt::WContainerWidget *w =
			dynamic_cast<Wt::WContainerWidget *>(editor);
		Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(w->widget(0));

		return Wt::cpp17::any(lineEdit->text());
	}

	void setEditState(Wt::WWidget *editor, const Wt::WModelIndex& index,
		const Wt::cpp17::any& value) const
	{
		Wt::WContainerWidget *w =
			dynamic_cast<Wt::WContainerWidget *>(editor);
		Wt::WLineEdit *lineEdit = dynamic_cast<Wt::WLineEdit *>(w->widget(0));

		lineEdit->setText(Wt::cpp17::any_cast<WT_USTRING>(value));
	}
};