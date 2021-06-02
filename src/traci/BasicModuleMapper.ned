package traci;

simple BasicModuleMapper like ModuleMapper
{
    parameters:
        @class(traci::BasicModuleMapper);
        string objectType;
        double penetrationRate = default(1.0);
}
