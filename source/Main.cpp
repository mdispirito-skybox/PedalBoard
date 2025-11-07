#include "MainComponent.h"

class GuitarAmpSimApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override       { return "Guitar Amp Sim"; }
    const juce::String getApplicationVersion() override    { return "0.1.0"; }
    void initialise(const juce::String&) override          { mainWindow.reset(new MainWindow(*this)); }
    void shutdown() override                               { mainWindow = nullptr; }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(GuitarAmpSimApplication& app)
            : DocumentWindow("Guitar Amp Sim",
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                             .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(GuitarAmpSimApplication)
