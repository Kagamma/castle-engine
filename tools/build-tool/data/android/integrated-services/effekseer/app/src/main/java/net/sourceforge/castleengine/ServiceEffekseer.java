package net.sourceforge.castleengine;

/**
 * Integration of ServiceEffekseer with Castle Game Engine on Android.
 */
public class ServiceEffekseer extends ServiceAbstract
{
    private static final String CATEGORY = "ServiceEffekseer";

    public ServiceEffekseer(MainActivity activity)
    {
        super(activity);
    }

    public String getName()
    {
        return "effekseer";
    }
}
